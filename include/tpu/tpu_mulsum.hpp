#pragma once
#include "core.hpp"

class IveTPUMulSum : public IveCore {
 public:
  virtual int init(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx) override;
  double getSum();

 protected:
  virtual int runSetup(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx,
                       const std::vector<bmk1880v2_tensor_tgmem_shape_t> &tg_in_slices,
                       const std::vector<bmk1880v2_tensor_tgmem_shape_t> &tg_out_slices,
                       std::vector<u32> *tl_in_idx, std::vector<u32> *tl_out_idx,
                       const bool enable_cext) override;
  virtual void operation(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx, u32 ping_idx) override;
  virtual void beforeSubmit(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx, std::vector<CviImg> &input,
                            std::vector<CviImg> *output) override;
  virtual int postProcess(bmctx_t *ctx) override;

 private:
  double m_sum = 1.f;
  std::vector<bmk1880v2_tensor_lmem_t *> m_input;
  bmk1880v2_tensor_lmem_t *mp_tl_mulsum = nullptr;
  bmk1880v2_tensor_lmem_shape_t m_tl_mulsum_shape;
  bmk1880v2_tensor_lmem_stride_t m_tl_mulsum_stride;
  bmk1880v2_tiu_element_wise_mul_param_t m_p_mul;

  bmmem_device_t m_bm_dev = NULL;
};