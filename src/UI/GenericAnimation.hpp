#pragma once

#include <functional>

class GenericAnimation
{
public:
	GenericAnimation(double durationMs);
	virtual ~GenericAnimation();

	virtual void OnUpdate(double ts);
	virtual void OnStop();
	virtual void OnReset();

	void SetProgressTime(double timeMs);

	// Will run before the first frame of the animation
	void SetOnStart(std::function<void(GenericAnimation*)> onStart);

	// Will run on frame when OnUpdate is called,
	// a pointer to the animation instance will be passed in as first argument
	void SetOnUpdate(std::function<void(GenericAnimation*)> onUpdate);

	// Will run at the end of the animation after the last frame of the animation,
	// a pointer to the animation instance will be passed in as first argument
	void SetOnEnd(std::function<void(GenericAnimation*)> onEnd);

	// Will run when the animation is cancelled,
	// a pointer to the animation instance will be passed in as first argument
	void SetOnStop(std::function<void(GenericAnimation*)> onStop);

	// Will run to set the animation into the initial state,
	// a pointer to the animation instance will be passed in as first argument
	void SetOnReset(std::function<void(GenericAnimation*)> onReset);

	const auto& IsRunning() const { return m_IsRunning; }
	const auto& GetDurationMs() const { return m_DurationMs; }
	const auto& GetLastTimeStep() const { return m_LastTimeStepMs; }
	const auto& GetProgressTimeMs() const { return m_ProgressTimeMs; }

	const double GetProgressNormalized() const;

private:
	std::function<void(GenericAnimation*)> m_OnStart{}, m_OnEnd{}, m_OnStop{}, m_OnReset{}, m_OnUpdate{};

	bool m_IsRunning{ false };
	double m_DurationMs{ 0.0f }, m_LastTimeStepMs{ 0.0f }, m_ProgressTimeMs{ 0.0f };
};