#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(FloatToIntMtd);

enum FloatToIntParams
{
   p_input = 0,
   p_mode
};

static const char* FloatToIntModeNames[] =
{
   "floor",
   "ceil",
   "round",
   NULL
};

enum FloatToIntMode
{
   FTI_FLOOR = 0,
   FTI_CEIL,
   FTI_ROUND
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterEnum("mode", 0, FloatToIntModeNames);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   FloatToIntMode mode = (FloatToIntMode) AiShaderEvalParamInt(p_mode);
   float input = AiShaderEvalParamFlt(p_input);
   
   switch (mode)
   {
   case FTI_CEIL:
      sg->out.INT = static_cast<int>(ceilf(input));
      return;
   case FTI_ROUND:
      sg->out.INT = static_cast<int>(floorf(0.5f * input));
      return;
   case FTI_FLOOR:
   default:
      sg->out.INT = static_cast<int>(floorf(input));
      return;
   }
}
