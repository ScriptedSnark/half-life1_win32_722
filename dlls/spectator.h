// Spectator.h

class CBaseSpectator : public CBaseEntity 
{
public:
	void Spawn();
	void SpectatorConnect(void);
	void SpectatorDisconnect(void);
	void SpectatorThink(void);

private:
	void SpectatorImpulseCommand(void);
};