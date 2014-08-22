#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SignC3Mtd);

enum SignC3Params
{
   p_input = 0
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
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
   AtRGB input = AiShaderEvalParamRGB(p_input);
   
   sg->out.RGB.r = float(SGN(input.r));
   sg->out.RGB.g = float(SGN(input.g));
   sg->out.RGB.b = float(SGN(input.b));
}
