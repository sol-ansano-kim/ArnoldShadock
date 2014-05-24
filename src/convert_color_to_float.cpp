#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ConvertColorToFloatMtd);

enum ConvertColorToFloatParams
{
   p_input = 0,
   p_mode
};

enum ConvertColorToFloatMode
{
   CTF_LUMINANCE = 0,
   CTF_INTENSITY,
   CTF_MAX,
   CTF_MIN
};

static const char* ConvertColorToFloatModeNames[] =
{
   "luminance",
   "intensity",
   "max",
   "min",
   NULL
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterENUM("mode", CTF_LUMINANCE, ConvertColorToFloatModeNames);
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
   static float sOneThird = 1.0f / 3.0f;
   
   AtRGB input = AiShaderEvalParamRGB(p_input);
   ConvertColorToFloatMode mode = (ConvertColorToFloatMode) AiShaderEvalParamInt(p_mode);
   
   switch (mode)
   {
   case CTF_INTENSITY:
      sg->out.FLT = (input.r + input.g + input.b) * sOneThird;
      break;
   case CTF_MAX:
      sg->out.FLT = MAX3(input.r, input.g, input.b);
      break;
   case CTF_MIN:
      sg->out.FLT = MIN3(input.r, input.g, input.b);
      break;
   case CTF_LUMINANCE:
   default:
      sg->out.FLT = 0.2126f * input.r + 0.7152f * input.g + 0.0722f * input.b;
   }
}
