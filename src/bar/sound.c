#include "bar/sound.h"

typedef struct {
  GtkImage *volume_icon;
  pa_glib_mainloop *pa_mloop;
  pa_context *pa_ctx;
} SoundContext;

// 3. Callback: Receives current audio data for the default sink
static void on_sink_info_received(pa_context *c, const pa_sink_info *i, int eol,
                                  gpointer user_data) {
  SoundContext *ctx = (SoundContext *)user_data;

  // End of list or invalid data point
  if (eol > 0 || !i)
    return;

  // Calculate volume level percentage
  pa_volume_t avg_vol = pa_cvolume_avg(&i->volume);
  int vol_percent = (int)((avg_vol * 100) / PA_VOLUME_NORM);
  gboolean is_muted = i->mute;

  const gchar *icon_name;
  if (is_muted) {
    icon_name = "audio-volume-muted-symbolic";
  } else if (vol_percent < 33) {
    icon_name = "audio-volume-low-symbolic";
  } else if (vol_percent < 66) {
    icon_name = "audio-volume-medium-symbolic";
  } else {
    icon_name = "audio-volume-high-symbolic";
  }

  gtk_image_set_from_icon_name(ctx->volume_icon, icon_name);
}

// 2. Callback: Triggered whenever system volume settings change
static void on_pa_subscription_event(pa_context *c,
                                     pa_subscription_event_type_t t,
                                     uint32_t idx, gpointer user_data) {
  if ((t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == PA_SUBSCRIPTION_EVENT_SINK) {
    pa_context_get_sink_info_by_name(c, "@DEFAULT_SINK@", on_sink_info_received,
                                     user_data);
  }
}

// 1. Callback: Triggered when PulseAudio/PipeWire-Pulse completes connection
// sync
static void on_pa_state_changed(pa_context *c, gpointer user_data) {
  SoundContext *ctx = (SoundContext *)user_data;
  pa_context_state_t state = pa_context_get_state(c);

  if (state == PA_CONTEXT_READY) {
    g_print("Connected to PulseAudio server backend safely.\n");

    // 1. Request initial default sink configurations
    pa_context_get_sink_info_by_name(c, "@DEFAULT_SINK@", on_sink_info_received,
                                     ctx);

    // 2. Subscribe to real-time events to handle hardware keys / external
    // mixers changing volume
    pa_context_set_subscribe_callback(c, on_pa_subscription_event, ctx);
    pa_context_subscribe(c, PA_SUBSCRIPTION_MASK_SINK, NULL, NULL);

  } else if (state == PA_CONTEXT_FAILED || state == PA_CONTEXT_TERMINATED) {
    g_printerr("PulseAudio context lost or disconnected.\n");
  }
}

void activateSound(GtkImage *volume_icon) {
  // Heap allocate the context metadata so it survives the scope of this
  // initialization function
  SoundContext *ctx = g_new0(SoundContext, 1);
  ctx->volume_icon = volume_icon;

  // Bind PulseAudio onto GTK's native context thread
  ctx->pa_mloop = pa_glib_mainloop_new(NULL);
  pa_mainloop_api *pa_api = pa_glib_mainloop_get_api(ctx->pa_mloop);

  // Create and initialize connection profiles
  ctx->pa_ctx = pa_context_new(pa_api, "GTK Volume Indicator");
  pa_context_set_state_callback(ctx->pa_ctx, on_pa_state_changed, ctx);

  // Establish non-blocking server synchronization loop
  if (pa_context_connect(ctx->pa_ctx, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
    g_printerr("Failed to execute non-blocking PulseAudio backend hook\n");
  }
}

// void activateSound() {
//     wp_init(WP_INIT_PIPEWIRE);
//
//     GMainLoop *loop = g_main_loop_new(NULL, false);
//
//     WpCore *core = wp_core_new(g_main_loop_get_context(loop), NULL, NULL);
//
//     if (!wp_core_is_connected(core)) {
//         printf("Failed to connect WpCore");
//         g_object_unref(core);
//         g_main_loop_unref(loop);
//     }
//
//     printf("Connected Successfully");
//
//     WpObjectManager *manager = wp_object_manager_new();
//
//     WpObjectInterest *interest = wp_object_interest_new_type(WP_TYPE_NODE);
//     wp_object_interest_add_constraint(interest,
//     WP_CONSTRAINT_TYPE_PW_PROPERTY, "media.class", "=s", "Audio/Sink");
// }