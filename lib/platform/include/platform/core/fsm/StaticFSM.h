/*
Platform - A C++ framework
*/

#pragma once
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#include "spdlog/spdlog.h"

#include <cstdlib>

namespace platform::core::fsm {
/**
 * @brief The template help implement a based-on-state-pattern-design FSM object. All states used with this FSM must
 * support enterState and leaveState methods. This use a pre-allocated memory area
 *
 * @tparam BaseState The base state of all states
 * @tparam StateSize The maximum size, in bytes, of the largest state used by FSM
 */
template <typename BaseState, std::size_t StateSize>
class StaticFSM {
public:
    StaticFSM() = default;
    virtual ~StaticFSM();

    /**
     * @brief Set the new state object. E.x. fsm.setState( new(fsm) NameOfNewState(fsm) )
     *
     * @param state
     */
    void setState(BaseState* state);

    void* select_memory_area(size_t size);

protected:
    /**
     * @brief Child class implementation, called when enter the state
     *
     */
    virtual void enteringState(BaseState*) {}

    /**
     * @brief Child class implementation, called before leaving the state
     *
     */
    virtual void leavingState(BaseState*) {}

    /**
     * @brief Get the state object
     *
     * @return BaseState*
     */
    BaseState* getState() const { return current_state; }

private:
#pragma pack(push, 1)
    uint8_t state[2][static_cast<size_t>(StateSize)]{};
#pragma pack(pop)
    BaseState* current_state = nullptr;
};

template <typename BaseState, std::size_t StateSize>
StaticFSM<BaseState, StateSize>::~StaticFSM()
{
    // Destroy any currently active state
    if (current_state) {
        current_state->~BaseState();
    }
}

template <typename BaseState, std::size_t StateSize>
void* StaticFSM<BaseState, StateSize>::select_memory_area(size_t size)
{
    if (size > StateSize) {
        SPDLOG_ERROR("{}: Attempted to activate state that is larger ({}) than the designated buffer ({})", "StaticFSM",
                     size, StateSize);
        abort();
    }

    // Get the memory not used by the active state.
    void* reclaimed = current_state == reinterpret_cast<void*>(&state[0][0]) ? &state[1][0] : &state[0][0];

    return reclaimed;
}

template <typename BaseState, std::size_t StateSize>
void StaticFSM<BaseState, StateSize>::setState(BaseState* new_state)
{
    if (current_state != nullptr) {
        leavingState(current_state);
        current_state->leaveState();
        current_state->~BaseState();
    }

    current_state = new_state;
    enteringState(current_state);
    current_state->enterState();
}
} // namespace platform::core::fsm

template <typename BaseState, std::size_t StateSize>
void* operator new(size_t size, platform::core::fsm::StaticFSM<BaseState, StateSize>& fsm)
{
    // Return the memory area to use for placement new.
    return fsm.select_memory_area(size);
}

template <typename BaseState, std::size_t StateSize>
void operator delete(void*, platform::core::fsm::StaticFSM<BaseState, StateSize>& fsm)
{
    fsm.select_memory_area(0);
}
