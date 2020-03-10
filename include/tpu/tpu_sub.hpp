#pragma once
#include "core.hpp"

class IveTPUSub : public IveCore {
 public:
  virtual int init(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx) override;
  virtual int runSetup(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx,
                       const std::vector<bmk1880v2_tensor_tgmem_shape_t> &tg_in_slices,
                       const std::vector<bmk1880v2_tensor_tgmem_shape_t> &tg_out_slices,
                       std::vector<u32> *tl_in_idx, std::vector<u32> *tl_out_idx,
                       const bool enable_cext) override;
  virtual void operation(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx, u32 ping_idx) override;

 private:
  std::vector<bmk1880v2_tensor_lmem_t *> m_input1;
  std::vector<bmk1880v2_tensor_lmem_t *> m_input2;
  bmk1880v2_tiu_element_wise_mac_param_t m_p_mac;
};

class IveTPUSubAbs : public IveCore {
 public:
  virtual int init(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx) override;
  virtual int runSetup(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx,
                       const std::vector<bmk1880v2_tensor_tgmem_shape_t> &tg_in_slices,
                       const std::vector<bmk1880v2_tensor_tgmem_shape_t> &tg_out_slices,
                       std::vector<u32> *tl_in_idx, std::vector<u32> *tl_out_idx,
                       const bool enable_cext) override;
  virtual void operation(bmctx_t *ctx, bmk1880v2_context_t *bk_ctx, u32 ping_idx) override;

 private:
  std::vector<bmk1880v2_tensor_lmem_t *> m_input1;
  std::vector<bmk1880v2_tensor_lmem_t *> m_input2;
  std::vector<bmk1880v2_tensor_lmem_t *> m_min;
  bmk1880v2_tiu_element_wise_max_param_t m_p_max;
  bmk1880v2_tiu_element_wise_min_param_t m_p_min;
  bmk1880v2_tiu_element_wise_mac_param_t m_p_mac;
};
