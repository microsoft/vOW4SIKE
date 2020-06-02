#include "../gen/points.hpp"
#include "../gen/random_function.hpp"
#include "../gen/instance.hpp"
#include "../memory/ram.hpp"
#include "../prng/aes.hpp"


template void print_all_threads
    <GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance>
    (
        const vOW<GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance> *vow,
        private_state_t<GenPoint, AESPRNG, GenRandomFunction<GenPoint>, GenInstance> &private_state,
        unsigned char *buffer
    );


// windowed sync
template bool windowed_resync_should_resync
    <GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance>
    (
        const vOW<GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance> *vow,
        private_state_t<GenPoint, AESPRNG, GenRandomFunction<GenPoint>, GenInstance> &private_state
    );

template void windowed_resync_do_resync
    <GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance>
    (
        const vOW<GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance> *vow,
        private_state_t<GenPoint, AESPRNG, GenRandomFunction<GenPoint>, GenInstance> &private_state
    );

template void windowed_resync
    <GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance>
    (
        const vOW<GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance> *vow,
        private_state_t<GenPoint, AESPRNG, GenRandomFunction<GenPoint>, GenInstance> &private_state
    );


// stakhanovist sync
template bool stakhanovist_resync_should_resync
    <GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance>
    (
        vOW<GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance> *S,
        private_state_t<GenPoint, AESPRNG, GenRandomFunction<GenPoint>, GenInstance> &private_state
    );

template void stakhanovist_resync_do_resync
    <GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance>
    (
        vOW<GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance> *S,
        private_state_t<GenPoint, AESPRNG, GenRandomFunction<GenPoint>, GenInstance> &private_state
    );


// nobiggie sync
template bool nobiggie_resync_should_resync
    <GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance>
    (
        vOW<GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance> *S,
        private_state_t<GenPoint, AESPRNG, GenRandomFunction<GenPoint>, GenInstance> &private_state
    );

template void nobiggie_resync_do_resync
    <GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance>
    (
        vOW<GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance> *S,
        private_state_t<GenPoint, AESPRNG, GenRandomFunction<GenPoint>, GenInstance> &private_state
    );
