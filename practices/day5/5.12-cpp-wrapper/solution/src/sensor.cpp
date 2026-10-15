#include <errno.h>

#include "sensor.hpp"

namespace training
{

Sensor::~Sensor()
{
	if (active_) {
		sensor_api_close(&handle_);
		active_ = false;
	}
}

InitStatus Sensor::init(Channel channel)
{
	if (active_) {
		return InitStatus::busy;
	}

	const auto c_channel = static_cast<sensor_api_channel>(channel);
	const int rc = sensor_api_open(&handle_, c_channel);

	if (rc == 0) {
		active_ = true;
		return InitStatus::ok;
	}
	if (rc == -EBUSY) {
		return InitStatus::busy;
	}
	if (rc == -EINVAL) {
		return InitStatus::invalid_channel;
	}
	return InitStatus::backend_error;
}

ReadResult Sensor::read()
{
	sensor_api_sample sample{};
	const int rc = sensor_api_read(&handle_, &sample);

	if (rc == 0) {
		return {
			ReadStatus::ok,
			{sample.value_milli, sample.sequence},
		};
	}
	if (rc == -EBUSY) {
		return {ReadStatus::busy, {0, 0U}};
	}
	if (rc == -EACCES) {
		return {ReadStatus::not_ready, {0, 0U}};
	}
	return {ReadStatus::io_error, {0, 0U}};
}

bool Sensor::ready() const
{
	return active_;
}

} // namespace training
