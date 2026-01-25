#include "score/mw/log/detail/utils/signal_handling/signal_handling.h"

namespace score::mw::log::detail
{

score::cpp::expected<std::int32_t, score::os::Error> SignalHandling::PThreadBlockSigTerm(score::os::Signal& signal) noexcept
{
    sigset_t sig_set;

    score::cpp::expected<std::int32_t, score::os::Error> return_error_result{};
    auto result = signal.SigEmptySet(sig_set);
    if (result.has_value())
    {
        // signal handling is tolerated by design. Argumentation: Ticket-101432
        // coverity[autosar_cpp14_m18_7_1_violation]
        result = signal.SigAddSet(sig_set, SIGTERM);
        if (result.has_value())
        {
            /* NOLINTNEXTLINE(score-banned-function) using PthreadSigMask by design. Argumentation: Ticket-101432 */
            result = signal.PthreadSigMask(SIG_BLOCK, sig_set);
        }
    }
    return result;
}

score::cpp::expected<std::int32_t, score::os::Error> SignalHandling::PThreadUnblockSigTerm(score::os::Signal& signal) noexcept
{
    sigset_t sig_set;

    score::cpp::expected<std::int32_t, score::os::Error> return_error_result{};
    auto result = signal.SigEmptySet(sig_set);
    if (result.has_value())
    {
        // signal handling is tolerated by design. Argumentation: Ticket-101432
        // coverity[autosar_cpp14_m18_7_1_violation]
        result = signal.SigAddSet(sig_set, SIGTERM);
        if (result.has_value())
        {
            /* NOLINTNEXTLINE(score-banned-function) using PthreadSigMask by design. Argumentation: Ticket-101432 */
            result = signal.PthreadSigMask(SIG_UNBLOCK, sig_set);
        }
    }
    return result;
}

}  //  namespace score::mw::log::detail
