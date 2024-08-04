#pragma once

class SpawnScreen
{
public:
	SpawnScreen();
	~SpawnScreen() {};

	void Render();
	void Show(bool bShow) { m_bEnabled = bShow; }

public:
	bool	m_bEnabled;
};