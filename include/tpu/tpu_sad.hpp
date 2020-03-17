#pragma once
#include "bmkernel_non_atomic.h"
#include "core.hpp"
#include "table_manager.hpp"

class IveTPUSAD : public IveCore {
 public:
  void setTblMgr(TblMgr *tblmgr);
  void outputThresholdOnly(bool value);
  void doThreshold(bool value);
  void setThreshold(const u16 threshold, const u8 min_val, const u8 max_val);
  void setWindowSize(const int window_size);
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
  TblMgr *mp_tblmgr = nullptr;
  CviImg *mp_table_pos_neg = nullptr;
  bool m_output_thresh_only = false;
  bool m_do_threshold = false;
  u16 m_threshold = 0;
  u8 m_min_value = 0;
  u8 m_max_value = 255;
  bmk1880v2_tiu_element_wise_max_param_t m_p_max;
  bmk1880v2_tiu_element_wise_min_param_t m_p_min;
  bmk1880v2_tiu_element_wise_sub_param_t m_p_sub;
  bmk1880v2_tiu_depthwise_convolution_param_t m_p_conv;
  bmk1880v2_tiu_element_wise_add_param_t m_p_add_thresh;
  bmk1880v2_tiu_non_atomic_mask_param_t m_p_mask;
};