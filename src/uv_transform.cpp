#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UVTransformMtd);

enum UVTransformParams
{
   p_input = 0,
   p_order,
   p_scale,
   p_scale_pivot,
   p_rotation,
   p_rotation_pivot,
   p_translation,
   p_transform_pivots,
   p_recompute_surface_uv_derivs
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterEnum(SSTR::order, TO_SRT, TransformOrderNames);
   AiParameterPnt2(SSTR::scale, 1.0f, 1.0f);
   AiParameterPnt2(SSTR::scale_pivot, 0.5f, 0.5f);
   AiParameterFlt(SSTR::rotation, 0.0f);
   AiParameterPnt2(SSTR::rotation_pivot, 0.5f, 0.5f);
   AiParameterPnt2(SSTR::translation, 0.0f, 0.0f);
   AiParameterBool(SSTR::transform_pivots, false);
   AiParameterBool(SSTR::recompute_surface_uv_derivs, false);
   
   AiMetaDataSetBool(mds, SSTR::order, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::transform_pivots, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::recompute_surface_uv_derivs, SSTR::linkable, false);
}

struct NodeData
{
   TransformOrder order;
   bool transformPivots;
   bool recomputeSurfaceUVDerivs;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   data->order = (TransformOrder) AiNodeGetInt(node, SSTR::order);
   data->transformPivots = AiNodeGetBool(node, SSTR::transform_pivots);
   data->recomputeSurfaceUVDerivs = AiNodeGetBool(node, SSTR::recompute_surface_uv_derivs);
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   AtPoint2 uv, dx, dy;
   AtVector dPdx, dPdy;
   float cosA, sinA;
   
   UVData uvs(sg);
   
   AtPoint2 S = AiShaderEvalParamPnt2(p_scale);
   AtPoint2 Sp = AiShaderEvalParamPnt2(p_scale_pivot);
   float R = AiShaderEvalParamFlt(p_rotation);
   AtPoint2 Rp = AiShaderEvalParamPnt2(p_rotation_pivot);
   AtPoint2 T = AiShaderEvalParamPnt2(p_translation);
   
   // start by inverting values... we're modifying the uvs, be we visualize the resulting mapped texture
   // feels more natural to manipulate the result
   S.x = 1.0f / S.x;
   S.y = 1.0f / S.y;
   R = -R;
   T = -T;
   
   if (data->recomputeSurfaceUVDerivs)
   {
      ComputeSurfaceScreenDerivatives(sg, dPdx, dPdy);
   }
   
   uv.x = sg->u;
   uv.y = sg->v;
   dx.x = sg->u + sg->dudx;
   dx.y = sg->v + sg->dvdx;
   dy.x = sg->u + sg->dudy;
   dy.y = sg->v + sg->dvdy;
   
   R *= AI_DTOR;
   cosA = cosf(R);
   sinA = sinf(R);
   
   switch (data->order)
   {
   case TO_SRT:
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      if (data->transformPivots) ScaleUV(Sp, S, Rp);
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      break;
   case TO_STR:
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      if (data->transformPivots) ScaleUV(Sp, S, Rp);
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      if (data->transformPivots) TranslateUV(T, Rp);
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      break;
   case TO_RST:
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      if (data->transformPivots) RotateUV(Rp, cosA, sinA, Sp);
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      break;
   case TO_RTS:
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      if (data->transformPivots) RotateUV(Rp, cosA, sinA, Sp);
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      if (data->transformPivots) TranslateUV(T, Sp);
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      break;
   case TO_TSR:
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      if (data->transformPivots) { TranslateUV(T, Sp); TranslateUV(T, Rp); }
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      if (data->transformPivots) ScaleUV(Sp, S, Rp);
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      break;
   case TO_TRS:
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      if (data->transformPivots) { TranslateUV(T, Sp); TranslateUV(T, Rp); }
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      if (data->transformPivots) RotateUV(Rp, cosA, sinA, Sp);
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      break;
   default:
      break;
   }
   
   sg->u = uv.x;
   sg->v = uv.y;
   sg->dudx = dx.x - uv.x;
   sg->dvdx = dx.y - uv.y;
   sg->dvdy = dy.x - uv.x;
   sg->dvdy = dy.y - uv.y;
   
   if (data->recomputeSurfaceUVDerivs)
   {
      // Re-compute dPdu and dPdv from dPdx, dPdy and new uv derivatives
      ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
   }
   
   sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   
   uvs.restore(sg);
}
