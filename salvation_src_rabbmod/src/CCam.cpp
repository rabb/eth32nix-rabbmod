// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"

#ifndef min
#define min( x, y ) ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#endif
#ifndef max
#define max( x, y ) ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )
#endif

CCam Cam;
bool drawingCam = false;
vector<refEntity_t> refEntities;

// buffalo: all rail colors are not adjustable yet until I finish the controls for it

const char *camTypeText[CAM_MAX] =
{
	"Mortar",
	"Mortar Follow",
	"Rifle",
	"Enemy",
	//"Mirror",
	//"Panzer",
	"Satchel",
	"Target",
	//"Drop Cam 1",
	//"Drop Cam 2",
	//"Drop Cam 3",
	//"Drop Cam 4",
};


void CCam::resetInfo(void)
{
	this->enemyNum = -1;
	VectorClear(Cam.gentityOrigin);
	VectorClear(Cam.dropAngles);
}

/***********
* Main Cam *
************/

void CCam::drawCam(float x, float y, float width, float height, refdef_t *refDef, qboolean crosshair)
{
	drawingCam = true;

	if (!eth32.cg.snap)
		return;

	for(int i=0; i < refEntities.size(); i++ )
	{
		refEntity_t* re = &refEntities.at(i);
		if(re->entityNum == eth32.cg.snap->ps.clientNum && eth32.cg.players[re->entityNum].infoValid && re->torsoFrameModel)
		{
			re->renderfx |= RF_DEPTHHACK | RF_NOSHADOW;
			re->renderfx &= ~RF_THIRD_PERSON;
			if (eth32.settings.teamShader1)
			{
				re->customShader = eth32.cg.media.Shader1[eth32.settings.teamShader1];
				memcpy(re->shaderRGBA, eth32.settings.colorTeam, sizeof(char[3]));
				re->shaderRGBA[3] = 0xff;
			}
		}
		Syscall.R_AddRefEntityToScene(&refEntities.at(i));
	}

	refDef->x = x;
	refDef->y = y;
	refDef->width = width;
	refDef->height = height;

	// Draw the spycam scene
	orig_syscall(CG_R_RENDERSCENE, refDef);

	// Draw border
	Draw.RawRect(x, y, width, height, GUI_FONTCOLOR1, 1);

	// Draw the crosshair
	#define CROSSHAIR_SIZE 4
	if (crosshair)
		Draw.RawRect(x + (width / 2) - (CROSSHAIR_SIZE / 2), y + (height / 2) - (CROSSHAIR_SIZE / 2) , CROSSHAIR_SIZE, CROSSHAIR_SIZE, colorRed, 1);
}


/*************
* Mirror Cam *
**************/
/*

void CCam::mirrorCam(camInfo_t *ci)
{
	if(IS_DEAD(eth32.cg.clientNum))
		return;

	// Set the view
	refdef_t camRefDef;
	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));

	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	// fov
	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;

	// view angle
	vec3_t camAngle;
	VectorCopy(eth32.cg.refdefViewAngles, camAngle);

	camAngle[YAW] -= 180.0f;	// Look behind
	camAngle[PITCH] = -camAngle[PITCH];
	AnglesToAxis(camAngle, camRefDef.viewaxis);

	VectorCopy(eth32.cg.refdef->vieworg, camRefDef.vieworg);

	// Draw the spycam
	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qfalse);
}
*/

/*************
* Mortar Cam *
**************/


void CCam::mortarCam(camInfo_t *ci)
{
	if (eth32.cg.snap->ps.ammo == 0)
		return;

// Set mortar trajectory from current view
	vec3_t angles, forward;
	VectorCopy(eth32.cg.refdef->viewaxis[ROLL], forward);
	VectorCopy(eth32.cg.snap->ps.viewangles, angles);
	angles[PITCH] -= 60.f;
	AngleVectors(angles, forward, NULL, NULL);
	forward[0] *= 3000 * 1.1f;
	forward[1] *= 3000 * 1.1f;
	forward[2] *= 1500 * 1.1f;

	trajectory_t mortarTrajectory;
	mortarTrajectory.trType = TR_GRAVITY;
	mortarTrajectory.trTime = eth32.cg.time;
	VectorCopy(eth32.cg.muzzle, mortarTrajectory.trBase);
	VectorCopy(forward, mortarTrajectory.trDelta);

	// Calculate mortar impact
	int timeOffset = 0;
	trace_t mortarTrace;
	vec3_t mortarImpact;
	VectorCopy(mortarTrajectory.trBase, mortarImpact);
	#define TIME_STEP 20
	while (timeOffset < 10000) {
		vec3_t nextPos;
		timeOffset += TIME_STEP;
		BG_EvaluateTrajectory(&mortarTrajectory, eth32.cg.time + timeOffset, nextPos, qfalse, 0);
		orig_CG_Trace(&mortarTrace, mortarImpact, 0, 0, nextPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);
		if ((mortarTrace.fraction != 1)
				// Stop if we hit sky
				&& !((mortarTrace.surfaceFlags & SURF_NODRAW) || (mortarTrace.surfaceFlags & SURF_NOIMPACT))
				&& (mortarTrace.contents != 0)) {
			break;
		}
		VectorCopy(nextPos, mortarImpact);
	}

	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));

	// kobject: add some angles
	vec3_t	dpos;
	vec3_t	camOrg;

	dpos[0] = eth32.cg.refdef->vieworg[0]-mortarImpact[0];
	dpos[1] = eth32.cg.refdef->vieworg[1]-mortarImpact[1];
	dpos[2] = 0.0f;
	VectorNormalizeFast( dpos );
	VectorCopy( mortarImpact, camOrg );
	VectorMA( camOrg, ci->distance * sinf(ci->angle * M_PI/180.0), zAxis, camOrg );
	VectorMA( camOrg, ci->distance * cosf(ci->angle * M_PI/180.0), dpos, camOrg );

	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;

	VectorCopy(camOrg, camRefDef.vieworg);

	vec3_t camAngle;
	VectorCopy(eth32.cg.refdefViewAngles, camAngle);
	camAngle[PITCH] = ci->angle;

	AnglesToAxis(camAngle, camRefDef.viewaxis);

	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qtrue);

	// Draw impact time
	sprintf(this->str, "^7Impact Time: ^b%.1f ^7seconds", (float)timeOffset / 1000.0f);
	Draw.Text(ci->x1 + (w / 2) - (TEXTWIDTH(this->str) / 2), ci->y1 + h - 22 , 0.24f, str, GUI_FONTCOLOR1, qfalse, qtrue, &eth32.cg.media.fontArial, true);

}


/**************
* Rifle Cam *
***************/


void CCam::rifleCam(camInfo_t *ci)
{
	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	// Set the view
	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));
	// fov
	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;
	// origin
	VectorCopy(Aimbot.lastImpact, camRefDef.vieworg);
	camRefDef.vieworg[ROLL] += ci->distance;
	// angle
	vec3_t camAngle;
	VectorCopy(eth32.cg.refdefViewAngles, camAngle);
	camAngle[PITCH] = ci->angle;
	AnglesToAxis(camAngle, camRefDef.viewaxis);

	// Draw the riflecam
	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qtrue);

	// Draw impact time
	sprintf(this->str, "^b%.1f ^7s", (float)Aimbot.flyTime / 1000.0f);
	Draw.Text(ci->x1 + (w / 2) - (TEXTWIDTH(this->str) / 2), ci->y1 + h - 22 , 0.24f, str, GUI_FONTCOLOR1, qfalse, qtrue, &eth32.cg.media.fontArial, true);
}

/*************
* Panzer Cam *
**************/
/*

void CCam::panzerCam(camInfo_t *ci)
{
	// Set panzer direction
	vec3_t forward;
	VectorCopy(Engine.forward, forward);

	// Set a far away end point
	vec3_t aheadPos;
	VectorMA(eth32.cg.muzzle, 65536.0f, forward, aheadPos);

	// Find panzer impact
	trace_t panzerTrace;
	orig_CG_Trace(&panzerTrace, eth32.cg.muzzle, NULL, NULL, aheadPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);

	// If no impact or hit sky don't draw cam
	if ((panzerTrace.fraction == 1.0f) || (panzerTrace.surfaceFlags & SURF_NOIMPACT))
		return;

	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	// Set the view
	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));
	// fov
	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;
	// angle
	AnglesToAxis(eth32.cg.refdefViewAngles, camRefDef.viewaxis);
	VectorCopy(panzerTrace.endpos, camRefDef.vieworg);
	// origin
	vec3_t stepBack;
	VectorMA(vec3_origin, ci->distance, forward, stepBack);
	VectorSubtract(camRefDef.vieworg, stepBack, camRefDef.vieworg);

	// Draw the panzer cam
	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qtrue);
}
*/

/**************
* Satchel Cam *
***************/


void CCam::satchelCam(camInfo_t *ci)
{
	int satchelEntityNum = Engine.findSatchel();

	// If don't find satchel or is out of range to fire
	if (satchelEntityNum == -1 || VectorDistance(eth32.cg.refdef->vieworg, eth32.cg.gentity[satchelEntityNum].lerpOrigin) > 2000)
		return;

	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	// Set the view
	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));
	// fov
	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;
	// origin
	VectorCopy(eth32.cg.gentity[satchelEntityNum].lerpOrigin, camRefDef.vieworg);

	camRefDef.vieworg[ROLL] += ci->distance;
	// view angle
	vec3_t camAngle;
	VectorCopy(eth32.cg.refdefViewAngles, camAngle);
	camAngle[PITCH] = ci->angle;
	AnglesToAxis(camAngle, camRefDef.viewaxis);

	// Draw the spycam
	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qfalse);
}



/**************
* Grenade Cam *
***************/

/*
void CCam::grenadeCam(camInfo_t *ci)
{

	vec3_t	angles, forward, right, viewpos;
	vec3_t	nextPos, lastPos;
	float	upangle = 0, pitch;

	bool once = false;
	trajectory_t grenadeTrajectory;
	grenadeTrajectory.trType = TR_GRAVITY;
	grenadeTrajectory.trTime = eth32.cg.time;

	VectorCopy(eth32.cg.refdef->viewaxis[YAW], right);
	VectorMA(eth32.cg.muzzle, -20, right, eth32.cg.muzzle);
	VectorCopy(eth32.cg.muzzle, grenadeTrajectory.trBase);
	grenadeTrajectory.trBase[ROLL] -= 30.0;



	VectorCopy(grenadeTrajectory.trBase, viewpos);
	VectorCopy(eth32.cg.snap->ps.viewangles, angles);
	VectorCopy(Engine.forward, forward);

	pitch = eth32.cg.refdefViewAngles[0];

	// JPW NERVE -- smoke grenades always overhand
	if( pitch >= 0 ) {
		forward[2] += 0.5f;
		// Used later in underhand boost
		pitch = 1.3f;
	}
	else {
		pitch = -pitch;
		pitch = min( pitch, 30 );
		pitch /= 30.f;
		pitch = 1 - pitch;
		forward[2] += (pitch * 0.5f);

		// Used later in underhand boost
		pitch *= 0.3f;
		pitch += 1.f;
	}

	VectorNormalizeFast( forward );			//	make sure forward is normalized

	upangle = -(eth32.cg.refdefViewAngles[0]);	//	this will give between	-90 / 90
	upangle = min(upangle, 50);
	upangle = max(upangle, -50);		//	now clamped to	-50 / 50	(don't allow firing straight up/down)
	upangle = upangle/100.0f;			//				   -0.5 / 0.5
	upangle += 0.5f;					//				    0.0 / 1.0

	if(upangle < .1)
		upangle = .1;

	upangle *= 900; // buffalo: check for weapon type and change upangle based on type
					//	so we can have cam/trace for all explosive types

	VectorMA(viewpos, 8, forward, viewpos);
	viewpos[2] -= 8;
	upangle *= pitch;
	SnapVector( viewpos );

	VectorScale(forward, upangle, forward);

	VectorCopy(forward, grenadeTrajectory.trDelta);
	SnapVector(grenadeTrajectory.trDelta);


	// Calculate rifle impact
	int timeOffset = 0;
	trace_t grenadeTrace;
	vec3_t grenadeImpact;
	VectorCopy(grenadeTrajectory.trBase, grenadeImpact);
	#define TIME_STEPP 50
	int maxTime = 3000;
	int totalFly = 0;

	while (timeOffset < maxTime) {
		timeOffset += TIME_STEPP;
		totalFly += TIME_STEPP;

		BG_EvaluateTrajectory(&grenadeTrajectory, eth32.cg.time + timeOffset, nextPos, qfalse, 0);
		orig_CG_Trace(&grenadeTrace, grenadeImpact, 0, 0, nextPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);

		// check for hit
		if (grenadeTrace.startsolid || grenadeTrace.fraction != 1) {
			// When a nade flies for over 750ms and hits somnexing, it'll explode
			if (totalFly > 1200)
				break;

			// calculate reflect angle (forward axis)
			int hitTime = eth32.cg.time + totalFly - TIME_STEPP + TIME_STEPP * grenadeTrace.fraction;
			BG_EvaluateTrajectoryDelta(&grenadeTrajectory, hitTime, grenadeTrajectory.trDelta, qfalse, 0);
			float dot = DotProduct(grenadeTrajectory.trDelta, grenadeTrace.plane.normal);
			VectorMA(grenadeTrajectory.trDelta, -2*dot, grenadeTrace.plane.normal, grenadeTrajectory.trDelta);

			VectorScale(grenadeTrajectory.trDelta, 0.35, grenadeTrajectory.trDelta);

			if (grenadeTrace.surfaceFlags == 0)
				VectorScale(grenadeTrajectory.trDelta, 0.5, grenadeTrajectory.trDelta);

			// calc new max time and reset trTime
			maxTime -= timeOffset;
			timeOffset = 0;
			grenadeTrajectory.trTime = eth32.cg.time;

			// new base origins
			VectorCopy(grenadeTrace.endpos, grenadeTrajectory.trBase);

			SnapVector(grenadeTrajectory.trDelta);
			SnapVector(grenadeTrajectory.trBase);
		} else {
			VectorCopy(nextPos, grenadeImpact);

			if(!once)
			{
				VectorCopy(nextPos, lastPos);
				once = true;
			}
			if(Engine.IsVisible(eth32.cg.refdef->vieworg, nextPos, 0))
       			Engine.MakeRailTrail( lastPos, nextPos, false, colorGreen, 160 );
			else
				Engine.MakeRailTrail( lastPos, nextPos, false, colorRed, 200 );

			VectorCopy(nextPos, lastPos);

		}
	}

	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	// Set the view
	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));
	// fov
	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;
	// origin
	VectorCopy(grenadeImpact, camRefDef.vieworg);
	camRefDef.vieworg[ROLL] += ci->distance;
	// angle
	vec3_t camAngle;
	VectorCopy(eth32.cg.refdefViewAngles, camAngle);
	camAngle[PITCH] = ci->angle;
	AnglesToAxis(camAngle, camRefDef.viewaxis);

	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qtrue);

}
*/

/********************
* Mortar Follow Cam *
*********************/


void CCam::followCam(camInfo_t *ci)
{

	vec3_t		forward, right, up, ViewAngles;
	refdef_t	camRefDef;

	vec3_t		focusAngles, focusPoint, view;

	float		forwardScale, sideScale, focusDist;

	if(!IS_WEAPATTRIB(eth32.cg.snap->ps.weapon, WA_MORTAR))
		return;

	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));

	VectorCopy( eth32.cg.refdefViewAngles, ViewAngles );
	VectorCopy( ViewAngles, camRefDef.vieworg );
	VectorCopy( ViewAngles, focusAngles );
	focusAngles[PITCH] = 45;

	AngleVectors( focusAngles, forward, NULL, NULL );

	VectorCopy( this->gentityOrigin, focusPoint );
	VectorCopy( this->gentityOrigin, view );
	view[2] += 8;

	ViewAngles[PITCH] *= 0.5;

	AngleVectors( ViewAngles, forward, right, up );

	forwardScale = cos( 0 / 180 * M_PI );
	sideScale = sin( 0 / 180 * M_PI );
	VectorMA( view, -ci->distance * forwardScale, forward, view );
	VectorMA( view, -ci->distance * sideScale, right, view );

	// select pitch to look at focus point from vieword
	VectorSubtract( focusPoint, view, focusPoint );
	focusDist = sqrt( focusPoint[0] * focusPoint[0] + focusPoint[1] * focusPoint[1] );

	if ( focusDist < 1 ) {
		focusDist = 1;	// should never happen
	}

	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	VectorCopy( view, camRefDef.vieworg );

	// fov
	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;

	//Adjusting the angle to look towards entity origin
	vec3_t temp;
	VectorCopy( ViewAngles, temp );

	temp[PITCH] = -180 / M_PI * atan2( focusPoint[2], focusDist );

	AnglesToAxis( temp, camRefDef.viewaxis );

	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qfalse);
}


/***********
* Drop Cam *
************//*

void CCam::dropCam(camInfo_t *ci)
{

	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	// Set the view
	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));
	// fov
	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;
	// origin
	VectorCopy(ci->origin, camRefDef.vieworg);

	// view angle
	vec3_t camAngle;

	if(enemyNum != -1)
	{
		Point(ci);
		VectorAdd(this->dropAngles, ci->axis, camAngle);
	}
	else
	{
		enemyNum = -1;
		VectorCopy(ci->axis, camAngle);
		camAngle[PITCH] = ci->angle;
		camRefDef.vieworg[ROLL] += ci->distance;
	}

	AnglesToAxis(camAngle, camRefDef.viewaxis);

	// Draw the spycam
	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qfalse);

	//if(enemyNum != -1)
		//sprintf_s(this->str, sizeof(this->str), eth32.cg.players[enemyNum].name);
		//Draw.Text(ci->x1 + (w / 2) - (TEXTWIDTH(this->str) / 2), ci->y1 + h - 22 , 0.24f, str, GUI_FONTCOLOR1, qfalse, qtrue, &eth32.cg.media.fontArial, true);
}

void CCam::dropCamSet(int camNum)
{
	camInfo_t *drop = &eth32.settings.cams[CAM_DROP_ONE + eth32.settings.dropCamCount];

	drop->active = true;

	VectorCopy(eth32.cg.refdef->vieworg, drop->origin );
	drop->origin[ROLL] += drop->distance;
	VectorCopy(eth32.cg.refdefViewAngles, drop->axis);


	if(camNum == 3)
		return;
	eth32.settings.dropCamCount++;
}

void CCam::dropCamKill(int camNum)
{
	camInfo_t *drop = &eth32.settings.cams[CAM_DROP_ONE + eth32.settings.dropCamCount];


	drop->active = false;

	VectorClear(drop->origin);
	VectorClear(drop->axis);

	if(camNum == 0)
		return;
	eth32.settings.dropCamCount--;

}

void CCam::dropCamActive( void )
{

	distance = 100000;
	enemyNum = -1;
	for(int i = 0; i < 4; i++)
	{
		int camNumber = CAM_DROP_ONE + i;
		camInfo_t *drop = &eth32.settings.cams[camNumber];

		if(drop->display && drop->active)
		{
			camDistance = VectorDistance(eth32.cg.refdef->vieworg, drop->origin);

			if(camDistance < distance)
			{
				distance = camDistance;
				closestCam = camNumber;
			}

			int distanceTwo = 100000;

			for(int i = 0; i < numFramePlayers; i++)
			{
				int clientNum = this->framePlayers[i];
				player_t *player = &eth32.cg.players[clientNum];
				float entDistance = VectorDistance(player->orHead.origin, drop->origin);

				if( !player->friendly
					&&	!VectorCompare(player->orHead.origin, vec3_origin)
					&& CheckFov(player->orHead.origin, drop) && (int)entDistance < distanceTwo)
				{
					distanceTwo = entDistance;
					enemyNum = player->clientNum;
				}
			}
		}
	}
}
*/
/*void CCam::Point(camInfo_t *ci)
{
	vec3_t org, ang;

	player_t *player = &eth32.cg.players[enemyNum];


	// Get the vector difference and calc the angle form it
	VectorSubtract(player->orHead.origin, ci->origin, org);
	Tools.VectorAngles(org, ang);

	// Clip & normalize the angle
    ang[PITCH] = -ang[PITCH];
	ANG_CLIP(ang[YAW]);
	ANG_CLIP(ang[PITCH]);

    ang[YAW] -= ci->axis[YAW];
	ANG_CLIP(ang[YAW]);
    ang[PITCH] -= ci->axis[PITCH];
	ANG_CLIP(ang[PITCH]);

	if (ang[YAW] < -180.0 || ang[YAW] > 180.0 || ang[PITCH] < -180.0 || ang[PITCH] > 180.0)
		return;

	this->dropAngles[YAW] += ang[YAW];
	this->dropAngles[PITCH] += ang[PITCH];
}*/

/*int CCam::CheckFov(vec3_t origin, camInfo_t *drop)
{
	if (drop->fov > 359.f)
		return 1;

	vec3_t dir;
	float ang;
	vec3_t viewaxis[3];
	AnglesToAxis(drop->axis, viewaxis);
	VectorSubtract( origin, drop->origin, dir );
	VectorNormalizeFast( dir );
	ang = 57.3*acos(DotProduct( dir, viewaxis[0] ));

	if (ang <= drop->fov)
		return 1;
	else
		return 0;
}*/

/************
* Enemy Cam *
*************/


int CCam::nearestEntity(void)
{

	distance = 100000;
	for(int i = 0; i < numFramePlayers; i++)
	{
		int clientNum = framePlayers[i];
		player_t *player = &eth32.cg.players[clientNum];

		float d = VectorDistance(eth32.cg.refdef->vieworg, player->orHead.origin);

		if(!player->friendly)
		{
			if(d < distance)
			{
				distance = d;
				return player->clientNum;
			}
		}
	}
	return -1;
}

void CCam::enemyCam(camInfo_t *ci)
{
	int entityNum = nearestEntity();

	if (entityNum == -1)
		return;

	player_t *player = &eth32.cg.players[entityNum];

	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	// Set the view
	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));
	// fov
	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;
	// origin
	VectorCopy( player->lerpOrigin, camRefDef.vieworg );

	if (player->currentState->eFlags & EF_CROUCHING)
		camRefDef.vieworg[2] += CROUCH_VIEWHEIGHT;
	else if (player->currentState->eFlags & EF_PRONE)
		camRefDef.vieworg[2] += PRONE_VIEWHEIGHT;
	else
		camRefDef.vieworg[2] += DEFAULT_VIEWHEIGHT;

	// view angle
	AnglesToAxis(player->currentState->apos.trBase, camRefDef.viewaxis);

	// Draw the spycam
	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qfalse);

	sprintf(this->str, player->name);
	Draw.Text(ci->x1 + (w / 2) - (TEXTWIDTH(this->str) / 2), ci->y1 + h - 22 , 0.24f, str, GUI_FONTCOLOR1, qfalse, qtrue, &eth32.cg.media.fontArial, true);
}

/*************
* Target Cam *
**************/

void CCam::targetCam(camInfo_t *ci)
{

	if (this->targetClientNum < 0 || this->targetClientNum > (MAX_CLIENTS-1) || !IS_INFOVALID(this->targetClientNum) || IS_DEAD(eth32.cg.clientNum))
	{
		this->targetClientNum = -1;
		return;
	}

	player_t *player = &eth32.cg.players[targetClientNum];

	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	// Set the view
	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));
	// fov
	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;
	// origin
	vec3_t origin;
	VectorCopy( player->orHead.origin, origin );
	VectorMA( origin, -(int)ci->distance, eth32.cg.refdef->viewaxis[0], origin );
	VectorCopy(origin, camRefDef.vieworg);

	// view angle
	vec3_t camAngle;

	VectorCopy(eth32.cg.refdefViewAngles, camAngle);
	AnglesToAxis(camAngle, camRefDef.viewaxis);

	// Draw the spycam
	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qfalse);

	strcpy(this->str, player->name);
	Draw.Text(ci->x1 + (w / 2) - (TEXTWIDTH(this->str) / 2), ci->y1 + h - 22 , 0.24f, str, GUI_FONTCOLOR1, qfalse, qtrue, &eth32.cg.media.fontArial, true);
}
