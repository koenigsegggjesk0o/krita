# Module Manifest

Complete inventory of the brush-engine extraction.

## File counts by area

| Area | Source files (.h/.cpp/.cc) | Total files |
|------|---------------------------:|------------:|
| `brush-engine/core/brush` | 63 | 92 |
| `brush-engine/core/brushengine` | 49 | 49 |
| `brush-engine/core/brush-mask` | 3 | 3 |
| `brush-engine/libpaintop` | 229 | 252 |
| `brush-engine/paintops` | 357 | 753 |
| `brush-presets` | 0 | 32 |
| `brush-tips` | 0 | 16 |
| `resources` | 136 | 185 |
| `dependencies/krita-libs` | 556 | 598 |
| `LICENSES` | 0 | 6 |
| `examples` | 2 | 3 |
| `docs` | 0 | 2 |

## Paintop engines (15)

| # | Engine dir | Main paintop header |
|---|------------|---------------------|
| 1 | `paintops/colorsmudge` | `kis_colorsmudgeop_settings_widget.h` |
| 2 | `paintops/curvebrush` | `KisCurveOpOptionData.h` |
| 3 | `paintops/defaultpaintops` | `KisBrushOpResources.h` |
| 4 | `paintops/deform` | `deform_brush.h` |
| 5 | `paintops/experiment` | `experiment_paintop_plugin.h` |
| 6 | `paintops/filterop` | `filterop.h` |
| 7 | `paintops/gridbrush` | `KisGridOpOptionWidget.h` |
| 8 | `paintops/hairy` | `KisHairyInkOptionWidget.h` |
| 9 | `paintops/hatching` | `KisHatchingOptionsModel.h` |
| 10 | `paintops/mypaint` | `MyPaintPaintOpPlugin.h` |
| 11 | `paintops/particle` | `particle_brush.h` |
| 12 | `paintops/roundmarker` | `kis_roundmarkerop.h` |
| 13 | `paintops/sketch` | `kis_sketch_paintop_settings.h` |
| 14 | `paintops/spray` | `KisSprayOpOptionData.h` |
| 15 | `paintops/tangentnormal` | `kis_tangent_normal_paintop_settings_widget.h` |

## Brush tip loaders (kritalibbrush)

- `KisAbrStorage.h`
- `KisBrushModel.h`
- `KisBrushServerProvider.h`
- `KisBrushTypeMetaDataFixup.h`
- `KisColorfulBrush.h`
- `kis_abr_brush.h`
- `kis_abr_brush_collection.h`
- `kis_auto_brush.h`
- `kis_auto_brush_factory.h`
- `kis_boundary.h`
- `kis_brush.h`
- `kis_brush_factory.h`
- `kis_brush_registry.h`
- `kis_brushes_pipe.h`
- `kis_dab_shape.h`
- `kis_gbr_brush.h`
- `kis_imagepipe_brush.h`
- `kis_pipebrush_parasite.h`
- `kis_png_brush.h`
- `kis_predefined_brush_factory.h`
- `kis_qimage_pyramid.h`
- `kis_scaling_size_brush.h`
- `kis_svg_brush.h`
- `kis_text_brush.h`
- `kis_text_brush_factory.h`

## Brush engine interfaces (kritaimage/brushengine)

- `KisOptimizedBrushOutline.h`
- `KisPaintOpPresetUpdateProxy.h`
- `KisPaintopSettingsIds.h`
- `KisPerStrokeRandomSource.h`
- `KisStrokeSpeedMeasurer.h`
- `brushengine.h`
- `kis_callback_based_paintop_property.h`
- `kis_callback_based_paintop_property_impl.h`
- `kis_combo_based_paintop_property.h`
- `kis_locked_properties.h`
- `kis_locked_properties_proxy.h`
- `kis_locked_properties_server.h`
- `kis_no_size_paintop_settings.h`
- `kis_paint_information.h`
- `kis_paintop.h`
- `kis_paintop_config_widget.h`
- `kis_paintop_factory.h`
- `kis_paintop_lod_limitations.h`
- `kis_paintop_preset.h`
- `kis_paintop_registry.h`
- `kis_paintop_settings.h`
- `kis_paintop_utils.h`
- `kis_slider_based_paintop_property.h`
- `kis_standard_uniform_properties_factory.h`
- `kis_stroke_random_source.h`
- `kis_uniform_paintop_property.h`

## Default brush presets (.kpp)

- `colorsmudge.kpp` — 59723 bytes
- `complex.kpp` — 692 bytes
- `curvebrush.kpp` — 9436 bytes
- `deformbrush.kpp` — 680 bytes
- `duplicate.kpp` — 59597 bytes
- `eraser.kpp` — 619 bytes
- `experimentbrush.kpp` — 681 bytes
- `filter.kpp` — 1238 bytes
- `gridbrush.kpp` — 625 bytes
- `hairybrush.kpp` — 769 bytes
- `hatchingbrush.kpp` — 59771 bytes
- `paintbrush.kpp` — 61845 bytes
- `particlebrush.kpp` — 497 bytes
- `roundmarker.kpp` — 1006 bytes
- `sketchbrush.kpp` — 1279 bytes
- `smudge.kpp` — 668 bytes
- `spraybrush.kpp` — 862 bytes
- `tangentnormal.kpp` — 9215 bytes

## Brush tip assets

### GIMP brush (.gbr)
- `brush-tips/test2.gbr`

### MyPaint brush (.myb) + previews
- `brush-tips/mypaint/c)_Pencil_1_Sketch_(mypaint).myb` (+ `c)_Pencil_1_Sketch_(mypaint)_prev.png`)
- `brush-tips/mypaint/c)_Pencil_2b_(mypaint).myb` (+ `c)_Pencil_2b_(mypaint)_prev.png`)
- `brush-tips/mypaint/d)_Ink_pen_(mypaint).myb` (+ `d)_Ink_pen_(mypaint)_prev.png`)
- `brush-tips/mypaint/e)_Marker_Medium_(mypaint).myb` (+ `e)_Marker_Medium_(mypaint)_prev.png`)
- `brush-tips/mypaint/e)_Marker_Plain_(mypaint).myb` (+ `e)_Marker_Plain_(mypaint)_prev.png`)
- `brush-tips/mypaint/i)_Wet_Knife_Plus_(mypaint).myb` (+ `i)_Wet_Knife_Plus_(mypaint)_prev.png`)
- `brush-tips/mypaint/i)_Wet_Paint_Plus_(mypaint).myb` (+ `i)_Wet_Paint_Plus_(mypaint)_prev.png`)

## Resource system key classes

- `KisBundleStorage.h`
- `KisDatabaseTransactionLock.h`
- `KisDirtyStateSaver.h`
- `KisEmbeddedResourceStorageProxy.h`
- `KisFolderStorage.h`
- `KisGlobalResourcesInterface.h`
- `KisLocalStrokeResources.h`
- `KisMemoryStorage.h`
- `KisRequiredResourcesOperators.h`
- `KisResourceCacheDb.h`
- `KisResourceIterator.h`
- `KisResourceLoader.h`
- `KisResourceLoaderRegistry.h`
- `KisResourceLocator.h`
- `KisResourceMetaDataModel.h`
- `KisResourceModel.h`
- `KisResourceModelProvider.h`
- `KisResourceQueryMapper.h`
- `KisResourceSearchBoxFilter.h`
- `KisResourceStorage.h`
- `KisResourceThumbnailCache.h`
- `KisResourceTypeModel.h`
- `KisResourceTypes.h`
- `KisResourcesInterface.h`
- `KisResourcesInterface_p.h`
- `KisSqlQueryLoader.h`
- `KisStorageFilterProxyModel.h`
- `KisStorageModel.h`
- `KisStoragePlugin.h`
- `KisTag.h`
- `KisTagFilterResourceProxyModel.h`
- `KisTagList.h`
- `KisTagModel.h`
- `KisTagModelProvider.h`
- `KisTagResourceModel.h`
- `KisTemporaryResourceStorageLock.h`
- `KoCanvasResourcesIds.h`
- `KoCanvasResourcesInterface.h`
- `KoEmbeddedResource.h`
- `KoLocalStrokeCanvasResources.h`
- `KoMD5Generator.h`
- `KoResource.h`
- `KoResourceBundle.h`
- `KoResourceBundleManifest.h`
- `KoResourceCacheInterface.h`
- `KoResourceCachePrefixedStorageWrapper.h`
- `KoResourceCacheStorage.h`
- `KoResourceLoadResult.h`
- `KoResourcePaths.h`
- `KoResourceServer.h`
- `KoResourceSignature.h`
- `ResourceDebug.h`
