#include "parameter.h"

const ParameterRecord parameterList[PARAM_NUM]=
{
//Type	Name				default value			min value				max value
	0,	"PWM_RATE",			{.intValue=		50},	{.intValue=		50},	{.intValue=		450},
	0,	"PWM_MIN",			{.intValue=		1000},	{.intValue=		0},		{.intValue=		2500},
	0,	"PWM_MAX",			{.intValue=		2000},	{.intValue=		0},		{.intValue=		2500},
	0,	"PWM_DISARMED",		{.intValue=		1000},	{.intValue=		0},		{.intValue=		2500},
	5,	"AAT_SIN_MAX",		{.floatValue=	3.0},	{.floatValue=	0},		{.floatValue=	5},
	5,	"AAT_SIN_MIN",		{.floatValue=	0.1},	{.floatValue=	0},		{.floatValue=	5},
	5,	"AAT_SIN_OFF",		{.floatValue=	0},		{.floatValue=	0},		{.floatValue=	5},
	5,	"AAT_COS_MAX",		{.floatValue=	3.0},	{.floatValue=	0},		{.floatValue=	5},
	5,	"AAT_COS_MIN",		{.floatValue=	0.1},	{.floatValue=	0},		{.floatValue=	5},
	5,	"AAT_COS_OFF",		{.floatValue=	0},		{.floatValue=	0},		{.floatValue=	5},
	0,	"AAT_SIN_CH",		{.intValue=		6},		{.intValue=		0},		{.intValue=		7},
	0,	"AAT_COS_CH",		{.intValue=		7},		{.intValue=		0},		{.intValue=		7},
	5,	"AAT_UP_ANG",		{.floatValue=	0},		{.floatValue=	-9},	{.floatValue=	9},
	5,	"AAT_DOWN_ANG",		{.floatValue=	0},		{.floatValue=	-9},	{.floatValue=	9},
};
