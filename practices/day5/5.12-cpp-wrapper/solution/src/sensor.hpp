#ifndef TRAINING_SENSOR_HPP_
#define TRAINING_SENSOR_HPP_

#include <cstddef>
#include <cstdint>

#include "sensor_api.h"

namespace training
{

enum class Channel : std::uint8_t {
	accel = SENSOR_API_CHANNEL_ACCEL,
	gyro = SENSOR_API_CHANNEL_GYRO,
};

enum class InitStatus : std::uint8_t {
	ok,
	busy,
	invalid_channel,
	backend_error,
};

enum class ReadStatus : std::uint8_t {
	ok,
	busy,
	io_error,
	not_ready,
};

struct Sample {
	std::int32_t value_milli;
	std::uint32_t sequence;
};

struct ReadResult {
	ReadStatus status;
	Sample sample;
};

struct SensorConfig {
	Channel channel;
	std::size_t read_attempts;
};

class Sensor final
{
      public:
	Sensor() = default;
	~Sensor();

	Sensor(const Sensor &) = delete;
	Sensor &operator=(const Sensor &) = delete;

	InitStatus init(Channel channel);
	ReadResult read();
	bool ready() const;

      private:
	sensor_api_handle handle_{};
	bool active_{false};
};

} // namespace training

#endif
