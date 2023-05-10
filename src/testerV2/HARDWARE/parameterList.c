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
	3,	"STK_CTRL_P",		{.floatValue=	1.0},	{.floatValue=	0},		{.floatValue=	100},
	3,	"STK_CTRL_I",		{.floatValue=	0.0},	{.floatValue=	0},		{.floatValue=	100},
	3,	"STK_CTRL_D",		{.floatValue=	0.0},	{.floatValue=	0},		{.floatValue=	100},
	5,	"STK_THRO_MIN",		{.floatValue=	0.0},	{.floatValue=	0},		{.floatValue=	1},
	5,	"STK_THRO_MAX",		{.floatValue=	1.0},	{.floatValue=	0},		{.floatValue=	1},
	2,	"STK_INT_MAX",		{.floatValue=	10.0},	{.floatValue=	0},		{.floatValue=	1000},
	5,	"STK_PHI_MAX",		{.floatValue=	0.74},	{.floatValue=	0},		{.floatValue=	1},
	0,	"SPL_CLK_RATE",		{.intValue=	1000},		{.intValue=	100},		{.intValue=	4000},
	0,	"SPL_ECLK_POL",		{.intValue=	0},			{.intValue=	0},			{.intValue=	1},
	0,	"SPL_EN_POL",		{.intValue=	0},			{.intValue=	0},			{.intValue=	1},
};
