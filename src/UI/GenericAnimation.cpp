#include "GenericAnimation.hpp"

GenericAnimation::GenericAnimation(double durationMs)
	: m_DurationMs(durationMs)
{
}

GenericAnimation::GenericAnimation(double durationMs, void* pCustomData)
    : m_DurationMs(durationMs), m_pCustomData(pCustomData)
{
}

GenericAnimation::~GenericAnimation() {
}

void GenericAnimation::OnUpdate(double ts) {
	if (!m_IsRunning && m_OnStart) {
		m_IsRunning = true;
		m_OnStart(this);
	}

	if (m_IsRunning && m_OnUpdate) {
		m_LastTimeStepMs = ts;
		m_ProgressTimeMs += m_LastTimeStepMs;
		m_OnUpdate(this);
	}

	if (m_ProgressTimeMs >= m_DurationMs && m_OnEnd) {
		m_OnEnd(this);
		m_ProgressTimeMs = 0.0f;
		m_IsRunning = false;
	}
}

void GenericAnimation::OnStop() {
	if (m_OnStop)
		m_OnStop(this);

	m_IsRunning = false;
}

void GenericAnimation::OnReset() {
	if (m_OnReset)
		m_OnReset(this);

	m_ProgressTimeMs = 0.0f;
	m_IsRunning = false;
}

void GenericAnimation::SetProgressTime(double timeMs) {
	m_ProgressTimeMs = timeMs;
}

void GenericAnimation::SetOnStart(std::function<void(GenericAnimation*)> onStart) {
	m_OnStart = onStart;
}

void GenericAnimation::SetOnUpdate(std::function<void(GenericAnimation*)> onUpdate) {
	m_OnUpdate = onUpdate;
}

void GenericAnimation::SetOnEnd(std::function<void(GenericAnimation*)> onEnd) {
	m_OnEnd = onEnd;
}

void GenericAnimation::SetOnStop(std::function<void(GenericAnimation*)> onStop) {
	m_OnStop = onStop;
}

void GenericAnimation::SetOnReset(std::function<void(GenericAnimation*)> onReset) {
	m_OnReset = onReset;
}

const double GenericAnimation::GetProgressNormalized() const {
	return std::min(m_ProgressTimeMs / m_DurationMs, 1.0);
}
