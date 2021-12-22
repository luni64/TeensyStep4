#pragma once

// Helper class to 'state-ify' legacy callback providers like e.g. DMAChannel, attachInterrupt, Intervaltimer...
// might go into a dedicated library

#include <array>
#include <utility>

template <typename Derived, typename callback_t, typename state_t, unsigned N>
class CallbackStateHelper
{
 public:
    static callback_t* callback[N];               // storage for the callback function pointers
    static state_t state[N];                      // storage for the corresponding state variables
    static const std::array<void (*)(), N> relay; // relay for attaching to an NVIC interrupt (type: void(*)())

 protected:
    template <unsigned nr>  // actual relay function #nr
    static void relayFunc() // relay simply invokes the stored callback with its corresponding state as parameter
    {
        callback[nr](state[nr]);
    }

    template <std::size_t... I>                                                      // Recursive helper function to generate an array of N void(*)() relay
    static constexpr std::array<void (*)(), N> MakeRelays(std::index_sequence<I...>) // the array will be intialized with (n=0..N) relay<n> functions.
    {
        return std::array<void (*)(), N>{relayFunc<I>...}; // The array will be generated and initialized at compile time
    }
};

// intialize static variables --------------------
template <typename Derived, typename callback_t, typename state_t, unsigned N>
callback_t* CallbackStateHelper<Derived, callback_t, state_t, N>::callback[N];

template <typename Derived, typename callback_t, typename state_t, unsigned N>
state_t CallbackStateHelper<Derived, callback_t, state_t, N>::state[N];

template <typename Derived, typename callback_t, typename state_t, unsigned N>
const std::array<void (*)(), N> CallbackStateHelper<Derived, callback_t, state_t, N>::relay = CallbackStateHelper::MakeRelays(std::make_index_sequence<N>{});
