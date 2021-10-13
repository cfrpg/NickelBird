#include "parameter.h"

const ParameterRecord parameterList[PARAM_NUM]=
{
//Type	Name				default value			min value				max value
	0,	"PWM_RATE",			{.intValue=		50},	{.intValue=		50},	{.intValue=		450},
	0,	"PWM_MIN",			{.intValue=		1000},	{.intValue=		0},		{.intValue=		2500},
	0,	"PWM_MAX",			{.intValue=		2000},	{.intValue=		0},		{.intValue=		2500},
	0,	"PWM_DISARMED",		{.intValue=		1000},	{.intValue=		0},		{.intValue=		2500},
	5,	"AAT_SIN_SCL",		{.floatValue=	3.0},	{.floatValue=	0},		{.floatValue=	100},	
	0,	"AAT_SIN_OFF",		{.intValue=		0},		{.intValue=		-32767},{.intValue=		32767},
	5,	"AAT_COS_SCL",		{.floatValue=	3.0},	{.floatValue=	0},		{.floatValue=	100},	
	0,	"AAT_COS_OFF",		{.intValue=		0},		{.intValue=		-32767},{.intValue=		32767},	
	5,	"AAT_UP_ANG",		{.floatValue=	0},		{.floatValue=	-9},	{.floatValue=	9},
	5,	"AAT_DOWN_ANG",		{.floatValue=	0},		{.floatValue=	-9},	{.floatValue=	9},
};
