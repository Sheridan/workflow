#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef CMS_DEBUG
  #define DF_SITE_URL "task-test.sergos.ru"
#else
  #define DF_SITE_URL "task.sergos.ru"
#endif

#define CMS_MEDIA_CROP_ICON_WIDTH  16
#define CMS_MEDIA_CROP_ICON_HEIGHT 16
#define CMS_MEDIA_CROP_THUMBNAIL_WIDTH  128
#define CMS_MEDIA_CROP_THUMBNAIL_HEIGHT 128
#define CMS_MEDIA_CROP_PREVIEW_WIDTH    320
#define CMS_MEDIA_CROP_PREVIEW_HEIGHT   240
#define CMS_MEDIA_CROP_FULL_SCALED_WIDTH    800
#define CMS_MEDIA_CROP_FULL_SCALED_HEIGHT   600

// ----------- tags --------------
#define CMS_TAG_SLIDER 14
#define CMS_TAG_TRIPLET 14
// ----------- tags --------------

#endif // GLOBAL_H
