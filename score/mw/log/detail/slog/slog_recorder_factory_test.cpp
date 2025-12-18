#include "gtest/gtest.h"

#include "score/mw/log/detail/slog/slog_recorder_factory.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

template <typename ConcreteRecorder>
bool IsRecorderOfType(const std::unique_ptr<Recorder>& recorder) noexcept
{
    static_assert(std::is_base_of<Recorder, ConcreteRecorder>::value,
                  "Concrete recorder shall be derived from Recorder base class");

    return dynamic_cast<const ConcreteRecorder*>(recorder.get()) != nullptr;
}

TEST(SlogRecorderFactoryTest, CreateRecorder)
{
    Configuration config;
    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();

    auto recorder = SlogRecorderFactory{}.CreateConcreteLogRecorder(config, memory_resource);

    // Slog uses TextRecorder
    EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
