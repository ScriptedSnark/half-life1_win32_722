//
//  hud_update.cpp
//

#include <math.h>
#include "hud.h"
#include "util.h"
#include <stdlib.h>



int CHud::UpdateClientData(client_data_t *cdata, float time)
{
	memcpy(m_vecOrigin, cdata->origin, sizeof(vec3_t));
	memcpy(m_vecAngles, cdata->viewangles, sizeof(vec3_t));
	m_iKeyBits = cdata->iKeyBits;
	m_iWeaponBits = cdata->iWeaponBits;
	gHUD.Think();

	cdata->iKeyBits = m_iKeyBits;
	cdata->fov = m_iFOV;

	// return 1 if in anything in the client_data struct has been changed, 0 otherwise
	return 1;
}


