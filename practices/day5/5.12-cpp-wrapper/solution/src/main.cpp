#include <cstddef>

#include <zephyr/sys/printk.h>

#include "sensor.hpp"

namespace
{

constexpr training::SensorConfig config{
	training::Channel::accel,
	5U,
};

const char *to_string(training::InitStatus status)
{
	switch (status) {
	case training::InitStatus::ok:
		return "ok";
	case training::InitStatus::busy:
		return "busy";
	case training::InitStatus::invalid_channel:
		return "invalid_channel";
	case training::InitStatus::backend_error:
		return "backend_error";
	}
	return "unknown";
}

const char *to_string(training::ReadStatus status)
{
	switch (status) {
	case training::ReadStatus::ok:
		return "ok";
	case training::ReadStatus::busy:
		return "busy";
	case training::ReadStatus::io_error:
		return "io_error";
	case training::ReadStatus::not_ready:
		return "not_ready";
	}
	return "unknown";
}

} // namespace

int main()
{
	{
		training::Sensor sensor;
		const auto init_status = sensor.init(config.channel);

		printk("CPP_SENSOR init=%s ready=%d attempts=%u\n", to_string(init_status),
		       sensor.ready(), static_cast<unsigned int>(config.read_attempts));

		if (init_status == training::InitStatus::ok) {
			for (std::size_t attempt = 0; attempt < config.read_attempts; ++attempt) {
				const auto result = sensor.read();

				printk("CPP_SENSOR read=%s sequence=%u value_milli=%d\n",
				       to_string(result.status), result.sample.sequence,
				       result.sample.value_milli);
			}
		}
	}

	training::Sensor reopened;
	const auto reopen_status = reopened.init(config.channel);
	printk("CPP_SENSOR reopen=%s ready=%d\n", to_string(reopen_status), reopened.ready());
	return 0;
}
