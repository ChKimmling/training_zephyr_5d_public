#include <errno.h>

#include "sensor.hpp"

namespace training
{

Sensor::~Sensor()
{
	/* TODO(5.12-01): Release an acquired C resource exactly once. */
}

InitStatus Sensor::init(Channel channel)
{
	/* TODO(5.12-02): Convert the strong channel type and call sensor_api_open(). */
	(void)channel;

	/* TODO(5.12-03): Translate the C return code and remember successful ownership. */
	return InitStatus::backend_error;
}

ReadResult Sensor::read()
{
	/* TODO(5.12-04): Read into a local C sample and return status plus value. */
	return {ReadStatus::not_ready, {0, 0U}};
}

bool Sensor::ready() const
{
	/* TODO(5.12-05): Report whether this object currently owns the resource. */
	return false;
}

} // namespace training
