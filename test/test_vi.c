#include<libtransistor/nx.h>

#define ASSERT_OK(label, expr) if((r = expr) != RESULT_OK) {            \
    dbg_printf("assertion failed at %s:%d: result 0x%x is not OK", __FILE__, __LINE__, r); \
    goto label;                                                         \
  }

int main() {
  svcSleepThread(100000000);

  result_t r;
  ASSERT_OK(fail, sm_init())
  ASSERT_OK(fail_sm, vi_init())

  dbg_printf("initialized vi");

  display_t display;
  ASSERT_OK(fail_vi, vi_open_display("Default", &display));

  surface_t surf;
  ASSERT_OK(fail_vi, vi_create_stray_layer(1, &display, &surf));

  dbg_printf("created stray layer");
  dbg_printf("  layer id: 0x%x", surf.layer_id);
  dbg_printf("  binder handle: 0x%x", surf.igbp_binder.handle);
  
 fail_vi:
  vi_finalize();
 fail_sm:
  sm_finalize();
 fail:
  return r != RESULT_OK;
}
