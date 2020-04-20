#pragma once
#include "core.hpp"

class IveTPUErode : public IveCore {
 public:
  void setKernel(IveKernel &kernel);
  virtual int init(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx) override;

 protected:
  virtual int runSetup(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx,
                       const std::vector<bmk1880v2_tensor_tgmem_shape_t> &tg_in_slices,
                       const std::vector<bmk1880v2_tensor_tgmem_shape_t> &tg_out_slices,
                       std::vector<u32> *tl_in_idx, std::vector<u32> *tl_out_idx,
                       const bool enable_cext) override;
  virtual void operation(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx, u32 ping_idx) override;
  virtual int freeChildTGMem(bmctx_t *ctx) override;

 private:
  IveKernel *m_kernel = nullptr;
  CviImg *mp_multiplier = nullptr;
  bmk1880v2_tensor_lmem_t *mp_xor_ones = nullptr;
  bmk1880v2_tiu_depthwise_convolution_qdm_param_t m_p_conv;
  bmk1880v2_tiu_element_wise_xor_int8_param_t m_p_xor;
};