#pragma once

#include <Syntacts/Config.hpp>
#include <Syntacts/Oscillator.hpp>
#include <Syntacts/Envelope.hpp>
#include <vector>
#include <utility>

namespace tact {

/// Encapsulates a cue to be played on a single channel
class SYNTACTS_API Cue {
public:

    /// Default constructor
    Cue();

    /// Constructs a Cue with a Oscillator lasting a specified duration
    Cue(std::shared_ptr<Oscillator> osc, float duration, float amplitude = 1.0f);

    /// Constructs a Cue with a Oscillator and Envelope follower
    Cue(std::shared_ptr<Oscillator> osc, std::shared_ptr<Envelope> env);

    /// Constructs a Cue with a Oscillator, modulating Oscillator, and Envelope follower
    Cue(std::shared_ptr<Oscillator> osc, std::shared_ptr<Oscillator> mod, std::shared_ptr<Envelope> env);

    /// Virtual destructor
    virtual ~Cue();

    /// Sets the primary Envelope of a Cue
    void setEnvelope(std::shared_ptr<Envelope> env);

    /// Gets the primary Envelope of a Cue
    std::shared_ptr<Envelope> getEnvelope() const;

    /// Chains an existing custom Generator to be processed
    void chain(std::shared_ptr<Generator> gen);

    /// Makes and then chains a custon Generator to be processed
    template <typename T, typename ...Args>
    void chain(Args ... args);

    /// Compounds all Generators to compute a sample at time t
    float sample(float t);

    /// Returns the number of samples this Cue generates for a particular sample rate
    int sampleCount(int sampleRate);

private:

    std::vector<std::shared_ptr<Generator>> m_generators; ///< array of generators
    std::shared_ptr<Envelope> m_env;                      ///< the Cue's primary envelope
};

template <typename T, typename ...Args>
void Cue::chain(Args... args) {
    auto g = std::make_shared<T>(std::forward<Args>(args)...);
    chain(std::move(g));
}

} // namespace tact