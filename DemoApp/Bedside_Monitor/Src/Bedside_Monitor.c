#if 1
/**
 * @file Bedside_Monitor.c
 * @brief A bedside monitoring demo application
 *
 * @author Bridgetek
 *
 * @date 2024
 *
 * MIT License
 *
 * Copyright (c) [2019] [Bridgetek Pte Ltd (BRTChip)]
 */

#include "Common.h"
#include "EVE_Platform.h"
#include "EVE_CoCmd.h"
#include "Bedside_Monitor.h"
#include "Helpers.h"
#include "Gesture.h"

EVE_HalContext s_halContext;
EVE_HalContext* s_pHalContext;
void SAMAPP_Bedside_Monitor();
#define SAMAPP_DELAY EVE_sleep(2000);
#define SCANOUT_FORMAT RGB8

// mapping file data
static const uint32_t arial_1_ASTC_glyph             []= {4096     , 5824  };
static const uint32_t arial_1_ASTC_xfont             []= {9920     , 176   };
static const uint32_t arial_1_ASTC_xfont_padding     []= {10096    , 16    };
static const uint32_t arial_2_ASTC_glyph             []= {10112    , 5824  };
static const uint32_t arial_2_ASTC_xfont             []= {15936    , 176   };
static const uint32_t arial_2_ASTC_xfont_padding     []= {16112    , 16    };
static const uint32_t arial_3_ASTC_glyph             []= {16128    , 5824  };
static const uint32_t arial_3_ASTC_xfont             []= {21952    , 176   };
static const uint32_t arial_3_ASTC_xfont_padding     []= {22128    , 16    };
static const uint32_t arial_4_ASTC_glyph             []= {22144    , 5824  };
static const uint32_t arial_4_ASTC_xfont             []= {27968    , 176   };
static const uint32_t arial_4_ASTC_xfont_padding     []= {28144    , 16    };
static const uint32_t arial_5_ASTC_glyph             []= {28160    , 5824  };
static const uint32_t arial_5_ASTC_xfont             []= {33984    , 176   };
static const uint32_t arial_5_ASTC_xfont_padding     []= {34160    , 16    };
static const uint32_t arial_6_ASTC_glyph             []= {34176    , 5824  };
static const uint32_t arial_6_ASTC_xfont             []= {40000    , 176   };
static const uint32_t arial_6_ASTC_xfont_padding     []= {40176    , 16    };
static const uint32_t arial_7_ASTC_glyph             []= {40192    , 5824  };
static const uint32_t arial_7_ASTC_xfont             []= {46016    , 176   };
static const uint32_t arial_7_ASTC_xfont_padding     []= {46192    , 16    };
static const uint32_t arial_8_ASTC_glyph             []= {46208    , 17472 };
static const uint32_t arial_8_ASTC_xfont             []= {63680    , 176   };
static const uint32_t arial_8_ASTC_xfont_padding     []= {63856    , 16    };
static const uint32_t arial_9_ASTC_glyph             []= {63872    , 17472 };
static const uint32_t arial_9_ASTC_xfont             []= {81344    , 176   };
static const uint32_t arial_9_ASTC_xfont_padding     []= {81520    , 16    };
static const uint32_t arial_10_ASTC_glyph            []= {81536    , 17472 };
static const uint32_t arial_10_ASTC_xfont            []= {99008    , 176   };
static const uint32_t arial_10_ASTC_xfont_padding    []= {99184    , 16    };
static const uint32_t arial_11_ASTC_glyph            []= {99200    , 17472 };
static const uint32_t arial_11_ASTC_xfont            []= {116672   , 176   };
static const uint32_t arial_11_ASTC_xfont_padding    []= {116848   , 16    };
static const uint32_t arial_12_ASTC_glyph            []= {116864   , 17472 };
static const uint32_t arial_12_ASTC_xfont            []= {134336   , 176   };
static const uint32_t arial_12_ASTC_xfont_padding    []= {134512   , 16    };
static const uint32_t arial_13_ASTC_glyph            []= {134528   , 23296 };
static const uint32_t arial_13_ASTC_xfont            []= {157824   , 176   };
static const uint32_t arial_13_ASTC_xfont_padding    []= {158000   , 16    };
static const uint32_t arial_14_ASTC_glyph            []= {158016   , 23296 };
static const uint32_t arial_14_ASTC_xfont            []= {181312   , 176   };
static const uint32_t arial_14_ASTC_xfont_padding    []= {181488   , 16    };
static const uint32_t arial_15_ASTC_glyph            []= {181504   , 23296 };
static const uint32_t arial_15_ASTC_xfont            []= {204800   , 176   };
static const uint32_t arial_15_ASTC_xfont_padding    []= {204976   , 16    };
static const uint32_t arial_16_ASTC_glyph            []= {204992   , 23296 };
static const uint32_t arial_16_ASTC_xfont            []= {228288   , 176   };
static const uint32_t arial_16_ASTC_xfont_padding    []= {228464   , 16    };
static const uint32_t arial_17_ASTC_glyph            []= {228480   , 23296 };
static const uint32_t arial_17_ASTC_xfont            []= {251776   , 176   };
static const uint32_t arial_17_ASTC_xfont_padding    []= {251952   , 16    };
static const uint32_t arial_18_ASTC_glyph            []= {251968   , 58240 };
static const uint32_t arial_18_ASTC_xfont            []= {310208   , 176   };
static const uint32_t arial_18_ASTC_xfont_padding    []= {310384   , 16    };
static const uint32_t arial_19_ASTC_glyph            []= {310400   , 58240 };
static const uint32_t arial_19_ASTC_xfont            []= {368640   , 176   };
static const uint32_t arial_19_ASTC_xfont_padding    []= {368816   , 16    };
static const uint32_t arial_20_ASTC_glyph            []= {368832   , 58240 };
static const uint32_t arial_20_ASTC_xfont            []= {427072   , 176   };
static const uint32_t arial_20_ASTC_xfont_padding    []= {427248   , 16    };
static const uint32_t arial_21_ASTC_glyph            []= {427264   , 58240 };
static const uint32_t arial_21_ASTC_xfont            []= {485504   , 176   };
static const uint32_t arial_21_ASTC_xfont_padding    []= {485680   , 16    };
static const uint32_t arial_22_ASTC_glyph            []= {485696   , 58240 };
static const uint32_t arial_22_ASTC_xfont            []= {543936   , 176   };
static const uint32_t arial_22_ASTC_xfont_padding    []= {544112   , 16    };
static const uint32_t arial_23_ASTC_glyph            []= {544128   , 52416 };
static const uint32_t arial_23_ASTC_xfont            []= {596544   , 176   };
static const uint32_t arial_23_ASTC_xfont_padding    []= {596720   , 16    };
static const uint32_t arial_24_ASTC_glyph            []= {596736   , 52416 };
static const uint32_t arial_24_ASTC_xfont            []= {649152   , 176   };
static const uint32_t arial_24_ASTC_xfont_padding    []= {649328   , 16    };
static const uint32_t arial_25_ASTC_glyph            []= {649344   , 52416 };
static const uint32_t arial_25_ASTC_xfont            []= {701760   , 176   };
static const uint32_t arial_25_ASTC_xfont_padding    []= {701936   , 16    };
static const uint32_t arial_26_ASTC_glyph            []= {701952   , 69888 };
static const uint32_t arial_26_ASTC_xfont            []= {771840   , 176   };
static const uint32_t arial_26_ASTC_xfont_padding    []= {772016   , 16    };
static const uint32_t arial_27_ASTC_glyph            []= {772032   , 81536 };
static const uint32_t arial_27_ASTC_xfont            []= {853568   , 176   };
static const uint32_t arial_27_ASTC_xfont_padding    []= {853744   , 16    };
static const uint32_t arial_28_ASTC_glyph            []= {853760   , 81536 };
static const uint32_t arial_28_ASTC_xfont            []= {935296   , 176   };
static const uint32_t arial_28_ASTC_xfont_padding    []= {935472   , 16    };
static const uint32_t arial_29_ASTC_glyph            []= {935488   , 81536 };
static const uint32_t arial_29_ASTC_xfont            []= {1017024  , 176   };
static const uint32_t arial_29_ASTC_xfont_padding    []= {1017200  , 16    };
static const uint32_t arial_30_ASTC_glyph            []= {1017216  , 81536 };
static const uint32_t arial_30_ASTC_xfont            []= {1098752  , 176   };
static const uint32_t arial_30_ASTC_xfont_padding    []= {1098928  , 16    };
static const uint32_t arial_31_ASTC_glyph            []= {1098944  , 81536 };
static const uint32_t arial_31_ASTC_xfont            []= {1180480  , 176   };
static const uint32_t arial_31_ASTC_xfont_padding    []= {1180656  , 16    };
static const uint32_t arial_32_ASTC_glyph            []= {1180672  , 93184 };
static const uint32_t arial_32_ASTC_xfont            []= {1273856  , 176   };
static const uint32_t arial_32_ASTC_xfont_padding    []= {1274032  , 16    };
static const uint32_t arial_33_ASTC_glyph            []= {1274048  , 93184 };
static const uint32_t arial_33_ASTC_xfont            []= {1367232  , 176   };
static const uint32_t arial_33_ASTC_xfont_padding    []= {1367408  , 16    };
static const uint32_t arial_34_ASTC_glyph            []= {1367424  , 93184 };
static const uint32_t arial_34_ASTC_xfont            []= {1460608  , 176   };
static const uint32_t arial_34_ASTC_xfont_padding    []= {1460784  , 16    };
static const uint32_t arial_35_ASTC_glyph            []= {1460800  , 104832};
static const uint32_t arial_35_ASTC_xfont            []= {1565632  , 176   };
static const uint32_t arial_35_ASTC_xfont_padding    []= {1565808  , 16    };
static const uint32_t arial_36_ASTC_glyph            []= {1565824  , 157248};
static const uint32_t arial_36_ASTC_xfont            []= {1723072  , 176   };
static const uint32_t arial_36_ASTC_xfont_padding    []= {1723248  , 16    };
static const uint32_t arial_37_ASTC_glyph            []= {1723264  , 157248};
static const uint32_t arial_37_ASTC_xfont            []= {1880512  , 176   };
static const uint32_t arial_37_ASTC_xfont_padding    []= {1880688  , 16    };
static const uint32_t arial_38_ASTC_glyph            []= {1880704  , 157248};
static const uint32_t arial_38_ASTC_xfont            []= {2037952  , 176   };
static const uint32_t arial_38_ASTC_xfont_padding    []= {2038128  , 16    };
static const uint32_t arial_39_ASTC_glyph            []= {2038144  , 157248};
static const uint32_t arial_39_ASTC_xfont            []= {2195392  , 176   };
static const uint32_t arial_39_ASTC_xfont_padding    []= {2195568  , 16    };
static const uint32_t arial_40_ASTC_glyph            []= {2195584  , 145600};
static const uint32_t arial_40_ASTC_xfont            []= {2341184  , 176   };
static const uint32_t arial_40_ASTC_xfont_padding    []= {2341360  , 16    };
static const uint32_t arial_41_ASTC_glyph            []= {2341376  , 145600};
static const uint32_t arial_41_ASTC_xfont            []= {2486976  , 176   };
static const uint32_t arial_41_ASTC_xfont_padding    []= {2487152  , 16    };
static const uint32_t arial_42_ASTC_glyph            []= {2487168  , 145600};
static const uint32_t arial_42_ASTC_xfont            []= {2632768  , 176   };
static const uint32_t arial_42_ASTC_xfont_padding    []= {2632944  , 16    };
static const uint32_t arial_43_ASTC_glyph            []= {2632960  , 174720};
static const uint32_t arial_43_ASTC_xfont            []= {2807680  , 176   };
static const uint32_t arial_43_ASTC_xfont_padding    []= {2807856  , 16    };
static const uint32_t arial_44_ASTC_glyph            []= {2807872  , 192192};
static const uint32_t arial_44_ASTC_xfont            []= {3000064  , 176   };
static const uint32_t arial_44_ASTC_xfont_padding    []= {3000240  , 16    };
static const uint32_t arial_45_ASTC_glyph            []= {3000256  , 192192};
static const uint32_t arial_45_ASTC_xfont            []= {3192448  , 176   };
static const uint32_t arial_45_ASTC_xfont_padding    []= {3192624  , 16    };
static const uint32_t arial_46_ASTC_glyph            []= {3192640  , 192192};
static const uint32_t arial_46_ASTC_xfont            []= {3384832  , 176   };
static const uint32_t arial_46_ASTC_xfont_padding    []= {3385008  , 16    };
static const uint32_t arial_47_ASTC_glyph            []= {3385024  , 192192};
static const uint32_t arial_47_ASTC_xfont            []= {3577216  , 176   };
static const uint32_t arial_47_ASTC_xfont_padding    []= {3577392  , 16    };
static const uint32_t arial_48_ASTC_glyph            []= {3577408  , 209664};
static const uint32_t arial_48_ASTC_xfont            []= {3787072  , 176   };
static const uint32_t arial_48_ASTC_xfont_padding    []= {3787248  , 16    };
static const uint32_t arial_49_ASTC_glyph            []= {3787264  , 209664};
static const uint32_t arial_49_ASTC_xfont            []= {3996928  , 176   };
static const uint32_t arial_49_ASTC_xfont_padding    []= {3997104  , 16    };
static const uint32_t arial_50_ASTC_glyph            []= {3997120  , 209664};
static const uint32_t arial_50_ASTC_xfont            []= {4206784  , 176   };
static const uint32_t arial_50_ASTC_xfont_padding    []= {4206960  , 16    };
static const uint32_t arial_51_ASTC_glyph            []= {4206976  , 209664};
static const uint32_t arial_51_ASTC_xfont            []= {4416640  , 176   };
static const uint32_t arial_51_ASTC_xfont_padding    []= {4416816  , 16    };
static const uint32_t arial_52_ASTC_glyph            []= {4416832  , 227136};
static const uint32_t arial_52_ASTC_xfont            []= {4643968  , 176   };
static const uint32_t arial_52_ASTC_xfont_padding    []= {4644144  , 16    };
static const uint32_t arial_53_ASTC_glyph            []= {4644160  , 302848};
static const uint32_t arial_53_ASTC_xfont            []= {4947008  , 176   };
static const uint32_t arial_53_ASTC_xfont_padding    []= {4947184  , 16    };
static const uint32_t arial_54_ASTC_glyph            []= {4947200  , 302848};
static const uint32_t arial_54_ASTC_xfont            []= {5250048  , 176   };
static const uint32_t arial_54_ASTC_xfont_padding    []= {5250224  , 16    };
static const uint32_t arial_55_ASTC_glyph            []= {5250240  , 302848};
static const uint32_t arial_55_ASTC_xfont            []= {5553088  , 176   };
static const uint32_t arial_55_ASTC_xfont_padding    []= {5553264  , 16    };
static const uint32_t arial_56_ASTC_glyph            []= {5553280  , 302848};
static const uint32_t arial_56_ASTC_xfont            []= {5856128  , 176   };
static const uint32_t arial_56_ASTC_xfont_padding    []= {5856304  , 16    };
static const uint32_t arial_57_ASTC_glyph            []= {5856320  , 285376};
static const uint32_t arial_57_ASTC_xfont            []= {6141696  , 176   };
static const uint32_t arial_57_ASTC_xfont_padding    []= {6141872  , 16    };
static const uint32_t arial_58_ASTC_glyph            []= {6141888  , 285376};
static const uint32_t arial_58_ASTC_xfont            []= {6427264  , 176   };
static const uint32_t arial_58_ASTC_xfont_padding    []= {6427440  , 16    };
static const uint32_t arial_59_ASTC_glyph            []= {6427456  , 285376};
static const uint32_t arial_59_ASTC_xfont            []= {6712832  , 176   };
static const uint32_t arial_59_ASTC_xfont_padding    []= {6713008  , 16    };
static const uint32_t arial_60_ASTC_glyph            []= {6713024  , 326144};
static const uint32_t arial_60_ASTC_xfont            []= {7039168  , 176   };
static const uint32_t arial_60_ASTC_xfont_padding    []= {7039344  , 16    };
static const uint32_t arial_61_ASTC_glyph            []= {7039360  , 349440};
static const uint32_t arial_61_ASTC_xfont            []= {7388800  , 176   };
static const uint32_t arial_61_ASTC_xfont_padding    []= {7388976  , 16    };
static const uint32_t arial_62_ASTC_glyph            []= {7388992  , 349440};
static const uint32_t arial_62_ASTC_xfont            []= {7738432  , 176   };
static const uint32_t arial_62_ASTC_xfont_padding    []= {7738608  , 16    };
static const uint32_t arial_63_ASTC_glyph            []= {7738624  , 349440};
static const uint32_t arial_63_ASTC_xfont            []= {8088064  , 176   };
static const uint32_t arial_63_ASTC_xfont_padding    []= {8088240  , 16    };
static const uint32_t arial_64_ASTC_glyph            []= {8088256  , 349440};
static const uint32_t arial_64_ASTC_xfont            []= {8437696  , 176   };
static const uint32_t arial_64_ASTC_xfont_padding    []= {8437872  , 16    };
static const uint32_t arial_65_ASTC_glyph            []= {8437888  , 372736};
static const uint32_t arial_65_ASTC_xfont            []= {8810624  , 176   };
static const uint32_t arial_65_ASTC_xfont_padding    []= {8810800  , 16    };
static const uint32_t arial_66_ASTC_glyph            []= {8810816  , 372736};
static const uint32_t arial_66_ASTC_xfont            []= {9183552  , 176   };
static const uint32_t arial_66_ASTC_xfont_padding    []= {9183728  , 16    };
static const uint32_t arial_67_ASTC_glyph            []= {9183744  , 372736};
static const uint32_t arial_67_ASTC_xfont            []= {9556480  , 176   };
static const uint32_t arial_67_ASTC_xfont_padding    []= {9556656  , 16    };
static const uint32_t arial_68_ASTC_glyph            []= {9556672  , 396032};
static const uint32_t arial_68_ASTC_xfont            []= {9952704  , 176   };
static const uint32_t arial_68_ASTC_xfont_padding    []= {9952880  , 16    };
static const uint32_t arial_69_ASTC_glyph            []= {9952896  , 495040};
static const uint32_t arial_69_ASTC_xfont            []= {10447936 , 176   };
static const uint32_t arial_69_ASTC_xfont_padding    []= {10448112 , 16    };
static const uint32_t arial_70_ASTC_glyph            []= {10448128 , 495040};
static const uint32_t arial_70_ASTC_xfont            []= {10943168 , 176   };
static const uint32_t arial_70_ASTC_xfont_padding    []= {10943344 , 16    };
static const uint32_t arialbd_70_ASTC_glyph          []= {10943360 , 495040};
static const uint32_t arialbd_70_ASTC_xfont          []= {11438400 , 176   };
static const uint32_t arialbd_70_ASTC_xfont_padding  []= {11438576 , 16    };
static const uint32_t ARIALNB_77_ASTC_glyph          []= {11438592 , 419328};
static const uint32_t ARIALNB_77_ASTC_xfont          []= {11857920 , 176   };
static const uint32_t ARIALNB_77_ASTC_xfont_padding  []= {11858096 , 16    };
static const uint32_t ARIALNB_85_ASTC_glyph          []= {11858112 , 495040};
static const uint32_t ARIALNB_85_ASTC_xfont          []= {12353152 , 176   };
static const uint32_t ARIALNB_85_ASTC_xfont_padding  []= {12353328 , 16    };
static const uint32_t ARIALNB_88_ASTC_glyph          []= {12361920 , 66560 };
static const uint32_t ARIALNB_88_ASTC_xfont          []= {12428480 , 176   };
static const uint32_t ARIALNB_88_ASTC_xfont_padding  []= {12428656 , 16    };


#define F_ADDR 0
#define F_SIZE 1

typedef struct {
	uint32_t flash_addr;
	uint32_t flash_size;

	uint32_t xfont_addr;
	uint32_t xfont_size;
	uint32_t handler;
	uint32_t* xfont;
	uint32_t cache_addr;
	uint32_t cache_size;
}app_font;

app_font font0 = { .xfont = &ARIALNB_88_ASTC_xfont };
app_font font1 = { .xfont = &arial_38_ASTC_xfont };
app_font font2 = { .xfont = &arial_25_ASTC_xfont };
app_font* fonts[] = {&font0, &font1, &font2 };
uint32_t zoom_in[] = { 0, 12428672,784 }; // structure: address on ramg (set later), address on flash, size
uint32_t zoom_out[] = { 0, 12429504,784 };

int main(int argc, char* argv[])
{
	s_pHalContext = &s_halContext;
	Gpu_Init(s_pHalContext);
#if defined(BT82X_ENABLE)
	LVDS_Config(s_pHalContext, SCANOUT_FORMAT, TESTCASE_PICTURE);
#endif

	// read and store calibration setting
#if !defined(BT8XXEMU_PLATFORM) && !defined(BT82X_ENABLE) && GET_CALIBRATION == 1
	Calibration_New(s_pHalContext);
#endif

	EVE_Util_clearScreen(s_pHalContext);

#if !defined(BT8XXEMU_PLATFORM)
	uint32_t sent = Flash_Init(s_pHalContext, TEST_DIR "ew2025_bedside_monitor_bt81x.bin", "ew2025_bedside_monitor_bt81x.bin", 0);
#endif

	char* info[] = { "EVE Sample Application",
					"This sample demonstrate a Beside Monitoring",
					"",
					"" };

	while (TRUE)
	{
		// WelcomeScreen(s_pHalContext, info);

		SAMAPP_Bedside_Monitor();

		EVE_Util_clearScreen(s_pHalContext);

		EVE_Hal_close(s_pHalContext);
		EVE_Hal_release();

		/* Init HW Hal for next loop*/
		Gpu_Init(s_pHalContext);
#if !defined(BT8XXEMU_PLATFORM) && !defined(BT82X_ENABLE) && GET_CALIBRATION == 1
		Calibration_Restore(s_pHalContext);
#endif
	}

	return 0;
}

/// The demo area //////////////////////////////////////////////////////////////////////////////////////
void load_app_assets(uint32_t ramg_offset) {

	// font addr calculation
	const uint8_t font_handler_start = 5;

	// prepare the font
	for (int i = 0; i < sizeof(fonts) / sizeof(app_font*); i++) {
		app_font* f = fonts[i];

		f->flash_addr = f->xfont[F_ADDR];
		f->flash_size = f->xfont[F_SIZE];
		f->handler = i + font_handler_start;
		f->xfont_addr = ALIGN_UP_TO_N( ramg_offset, 32); // SetFont2 32 bit align
		f->xfont_size = f->flash_size;
		ramg_offset = f->xfont_addr + f->xfont_size;
	
		// prepare font handler
		Gpu_CoCmd_Dlstart(s_pHalContext);
		App_WrCoCmd_Buffer(s_pHalContext, CLEAR(1, 1, 1));
		EVE_CoCmd_flashRead(s_pHalContext, f->xfont_addr, f->flash_addr, f->flash_size);
		Gpu_CoCmd_SetFont2(s_pHalContext, f->handler, f->xfont_addr, 0);
		Gpu_CoCmd_Swap(s_pHalContext);
		App_Flush_Co_Buffer(s_pHalContext);
		Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
	}

	// zoom icon
	zoom_in[0] = ramg_offset;
	zoom_out[0] = zoom_in[0] + zoom_in[2];
	EVE_CoCmd_flashRead(s_pHalContext, zoom_in[0], zoom_in[1], zoom_in[2]);
	EVE_CoCmd_flashRead(s_pHalContext, zoom_out[0], zoom_out[1], zoom_out[2]);

	// cahcing the font, only BT817/8, BT82x incompatible
	ramg_offset += zoom_in[2] + zoom_out[2];
	uint32_t ramg_remain = RAM_G_SIZE - ramg_offset;
	uint32_t num_font = sizeof(fonts) / sizeof(app_font*);
	uint32_t cache_size_per_font = max(16*1024, ALIGN_UP_TO_N(ramg_remain / num_font, 4));
	for (int i = 0; i < num_font; i++) {
		app_font* f = fonts[i];
		
		f->cache_addr = ALIGN_UP_TO_N(ramg_offset, 64);
		f->cache_size = cache_size_per_font;
		EVE_CoCmd_fontCache(s_pHalContext, f->handler, f->cache_addr, f->cache_size);
		ramg_offset = f->cache_addr + cache_size_per_font;
	}

	// test font
#if 0
	for (int i = 0; i < sizeof(fonts) / sizeof(app_font*); i++) {
		app_font* f = fonts[i];
		Gpu_CoCmd_Dlstart(s_pHalContext);
		App_WrCoCmd_Buffer(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
		App_WrCoCmd_Buffer(s_pHalContext, CLEAR(1, 1, 1));
		App_WrCoCmd_Buffer(s_pHalContext, COLOR_RGB(255, 255, 255));
		Gpu_CoCmd_Text(s_pHalContext, 50, 50, f->handler, 0, u8"ABCDEFGHIJKLMNOPQRSTUVWXYZ.0123456789! @#$%^&*()-_=+`~[{]};:'\"\\ | , <> / ? abcdefghijklmnopqrstuvwxyz");
		App_WrCoCmd_Buffer(s_pHalContext, DISPLAY());
		Gpu_CoCmd_Swap(s_pHalContext);
		App_Flush_Co_Buffer(s_pHalContext);
		Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
		EVE_sleep(200);
	}
#endif
}

void draw_app_window(app_box app_window)
{
	int border = 5;
	int bx = app_window.x;
	int by = app_window.y;
	int bw = app_window.w + border;
	int bh = app_window.h + border;
	char border_color[3] = { 134, 134, 134 };

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_DRAW_AT(0, 0);
	EVE_DRAW_AT(s_pHalContext->Width, s_pHalContext->Height);
	return;

	// Camera border
	// start draw a small rectangle with alpha value 255
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ZERO, ZERO));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_DRAW_AT(bx + border, by + border);
	EVE_DRAW_AT(bx + bw - border, by + bh - border);

	// finally draw the bigger rECTANGE he desired rgb valu);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(border_color[0], border_color[1], border_color[2]));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	EVE_Cmd_wr32(s_pHalContext, TAG(30));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_DRAW_AT(bx, by);
	EVE_DRAW_AT(bx + bw, by + bh);
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));

	EVE_CoCmd_number(s_pHalContext, app_window.x_mid, app_window.y - 30, 30, OPT_CENTERX, app_window.w);
	EVE_CoCmd_number(s_pHalContext, app_window.x - 40, app_window.y_mid, 30, OPT_CENTERX, app_window.h);
}

#define ENABLE_SCREENSHOT_CAPTURE 0

#define USE_BITMAP_L1                  0
#define USE_BITMAP_L1_ROTATE           1
#define USE_BITMAP_PALETTED_ROTATE     2
#define USE_BITMAP_BARGRAPH            3
#define USE_BITMAP_LINESTRIP           4

#define USEBITMAP USE_BITMAP_L1_ROTATE

#if  USEBITMAP == USE_BITMAP_L1
#define GRAPH_INIT graph_l1_init
#define GRAPH_DRAW graph_l1_draw

#elif  USEBITMAP == USE_BITMAP_L1_ROTATE
#define GRAPH_INIT graph_l1_rotate_init
#define GRAPH_DRAW graph_l1_rotate_draw

#elif  USEBITMAP == USE_BITMAP_PALETTED_ROTATE
#define GRAPH_INIT graph_p8_rotate_init
#define GRAPH_DRAW graph_p8_rotate_draw

#elif  USEBITMAP == USE_BITMAP_BARGRAPH
#define GRAPH_INIT graph_bargraph_init
#define GRAPH_DRAW graph_bargraph_draw

#elif  USEBITMAP == USE_BITMAP_LINESTRIP
#define GRAPH_INIT graph_linestrip_init
#define GRAPH_DRAW graph_linestrip_draw

#endif

uint32_t GRAPH_INIT(app_box* box_heartbeat, app_box* box_pleth, app_box* box_co2);
void GRAPH_DRAW();

int g_graph_zoom_lv = 3;

app_box box_menu_top;
app_box box_ecg;
app_box box_pth;
app_box box_co2;
app_box box_menu_bottom;

app_box box_graph_ecg;
app_box box_graph_pth;
app_box box_graph_co2;

char* const btnStartTxtActive = "START";
char* const btnStartTxtInActive = "STOP";
char* btnStartTxt = 0;
uint8_t btnStartState = BTN_START_INACTIVE;
uint32_t grid_bytes = 0;
uint32_t graph_size_ramg = 0;

void process_event() {
#if defined(BT82X_ENABLE)
	return; // disable touch on bt820
#endif

#if  USEBITMAP == USE_BITMAP_BARGRAPH
	return; // disable touch on bargraph
#endif

	Gesture_Touch_t* ges = Gesture_Renew(s_pHalContext);
	if (ges->tagReleased == TAG_ZOOM_DOWN) {
		g_graph_zoom_lv--;
		g_graph_zoom_lv = max(g_graph_zoom_lv, 1);
		//GRAPH_INIT(&box_ecg, &box_graph_pleth, &box_graph_co2);
	}

	else if (ges->tagReleased == TAG_ZOOM_UP) {
		g_graph_zoom_lv++;
		g_graph_zoom_lv = min(g_graph_zoom_lv, GRAPH_ZOOM_LV_MAX);
		//GRAPH_INIT(&box_ecg, &box_graph_pleth, &box_graph_co2);
	}

	else if (ges->tagReleased == TAG_START_STOP) {
		btnStartState = !btnStartState;
		if (BTN_START_ACTIVE == btnStartState) {
			btnStartTxt = btnStartTxtActive;
		}
		else {
			btnStartTxt = btnStartTxtInActive;
		}
	}

}

void draw_grid_by_cocmd(int32_t x, int32_t y, int32_t w, int32_t h, int32_t cell_w, int32_t cell_h) {
	const uint32_t ACC_FACTOR = 100;  // accuracy factor to simulate 0.01 pixel precision

	// Multiply all coordinates by ACC_FACTOR
	x *= ACC_FACTOR;
	y *= ACC_FACTOR;
	w *= ACC_FACTOR;
	h *= ACC_FACTOR;
	cell_w *= ACC_FACTOR;
	cell_h *= ACC_FACTOR;

	// Compute the number of columns and rows
	uint32_t num_cols = w / cell_w;
	uint32_t num_rows = h / cell_h;

	// Adjust cell size to fit perfectly
	cell_w = w / num_cols;
	cell_h = h / num_rows;

	uint32_t y_end = y + h;
	uint32_t x_end = x + w;

	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(6));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(80));

	// Draw vertical lines
	for (uint32_t i = 0; i <= num_cols; i++) {
		uint32_t line_x = x + i * cell_w;
		DRAW_LINE_XY(line_x / ACC_FACTOR, y / ACC_FACTOR, line_x / ACC_FACTOR, y_end / ACC_FACTOR);
	}

	// Draw horizontal lines
	for (uint32_t j = 0; j <= num_rows; j++) {
		uint32_t line_y = y + j * cell_h;
		DRAW_LINE_XY(x / ACC_FACTOR, line_y / ACC_FACTOR, x_end / ACC_FACTOR, line_y / ACC_FACTOR);
	}
}

void draw_grid_box(app_box box_graph) {
	int x = box_graph.x;
	int y = box_graph.y;
	int w = box_graph.w;
	int h = box_graph.h;
	int cell_w = 15;
	int cell_h = 15;
	draw_grid_by_cocmd(x, y, w, h, cell_w, cell_h);
}

void SAMAPP_Bedside_Monitor()
{
#if  USEBITMAP == USE_BITMAP_BARGRAPH
	g_graph_zoom_lv = 1; // disable touch on bargraph
#endif

	btnStartTxt = btnStartTxtInActive;

	app_box lcd_size = { 0, 0, s_pHalContext->Width, s_pHalContext->Height, s_pHalContext->Width, s_pHalContext->Height };
	app_box app_window = INIT_APP_BOX((s_pHalContext->Width - WINDOW_W) / 2, (s_pHalContext->Height - WINDOW_H) / 2, WINDOW_W, WINDOW_H);
	int graph_start = app_window.x + 35;

	box_menu_top = INIT_APP_BOX(app_window.x, app_window.y, app_window.w * 80 / 100, app_window.h * 8 / 100);
	box_ecg = INIT_APP_BOX(graph_start, box_menu_top.y_end, box_menu_top.w, app_window.h * 28 / 100);
	box_pth = INIT_APP_BOX(graph_start, box_ecg.y_end, box_menu_top.w, box_ecg.h);
	box_co2 = INIT_APP_BOX(graph_start, box_pth.y_end, box_menu_top.w, box_ecg.h);
	box_menu_bottom = INIT_APP_BOX(app_window.x, box_co2.y_end, box_menu_top.w, box_menu_top.h);

	int graph_margin_t = box_ecg.h / 4;
	int graph_margin_l = box_ecg.w / 40;
	int graph_margin_r = 5;
	int graph_margin_b = box_ecg.h / 10;
	int graph_margin_w = graph_margin_l + graph_margin_r + graph_start;
	int graph_margin_h = graph_margin_t + graph_margin_b;

	box_graph_ecg = INIT_APP_BOX(box_ecg.x + graph_margin_l, box_ecg.y + graph_margin_t, box_ecg.w - graph_margin_w, box_ecg.h - graph_margin_h);
	box_graph_pth = INIT_APP_BOX(box_pth.x + graph_margin_l, box_pth.y + graph_margin_t, box_pth.w - graph_margin_w, box_pth.h - graph_margin_h);
	box_graph_co2 = INIT_APP_BOX(box_co2.x + graph_margin_l, box_co2.y + graph_margin_t, box_co2.w - graph_margin_w, box_co2.h - graph_margin_h);

	int x = box_menu_top.x_end;
	int w = app_window.w - box_menu_top.w;
	int h = (app_window.h - box_menu_top.h - box_menu_bottom.h) / 4;
	int y = box_menu_top.y_end;
	int border = 2;

	app_box box_right1 = INIT_APP_BOX(x, y + h * 0+1, w, h);
	app_box box_right2 = INIT_APP_BOX(x, y + h * 1, w, h);
	app_box box_right3 = INIT_APP_BOX(x, y + h * 2, w, h);
	app_box box_right4 = INIT_APP_BOX(x, y + h * 3, w, h);

	init_datetime(11, 12, 2024, 9, 11, 0, 0);

	int time_start_ms = 0;
	int val_hr = 66;
	int val_spo2 = 96;
	int val_co2 = 22;
	int val_sys = 156;
	int val_dias = 93;

#define FONT_32 2 // note: BT81x maximum handler is 31
#define FONT_33 3
#define FONT_34 4
	static int screenshot_counter = 0;

	graph_size_ramg = GRAPH_INIT(&box_graph_ecg, &box_graph_pth, &box_graph_co2);
	load_app_assets(graph_size_ramg);

	while (1)
	{
		process_event();
		Display_Start(s_pHalContext);
#if defined(BT82X_ENABLE)
		EVE_CoCmd_sync(s_pHalContext);
#endif
		EVE_Cmd_wr32(s_pHalContext, VERTEX_FORMAT(EVE_VERTEX_FORMAT));

		draw_app_window(app_window);

		// the grid
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(100));
		draw_grid_box(box_graph_ecg);
		draw_grid_box(box_graph_pth);
		draw_grid_box(box_graph_co2);
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(16)); // reset line_width, without this, graphics become transparent 50%
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));

		GRAPH_DRAW();

		// Top menu box
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(43, 132, 135));
		DRAW_BOX(box_menu_top);
		// Bottom menu box
		DRAW_BOX(box_menu_bottom);

		int graph_border = 3;
		int brace_y = box_ecg.h / 7, brace_w = 5, brace_h = box_ecg.h * 4 / 5;

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		// Graph ECG braces outside
		DRAW_RECT(box_ecg.x, box_ecg.y + brace_y, brace_w, brace_h);
		// Graph pleth braces outside
		DRAW_RECT(box_pth.x, box_pth.y + brace_y, brace_w, brace_h);
		// Graph co2 braces outside
		DRAW_RECT(box_co2.x, box_co2.y + brace_y, brace_w, brace_h);

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		// Graph ecg braces inside
		DRAW_RECT(box_ecg.x + graph_border, box_ecg.y + brace_y + border, brace_w, brace_h - border * 2);
		// Graph pleth braces inside
		DRAW_RECT(box_pth.x + graph_border, box_pth.y + brace_y + border, brace_w, brace_h - border * 2);
		// Graph co2 braces inside
		DRAW_RECT(box_co2.x + graph_border, box_co2.y + brace_y + border, brace_w, brace_h - border * 2);

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(43, 132, 135));
		// right menu top
		DRAW_RECT(box_menu_top.x_end, box_menu_top.y, 1280 - box_menu_top.w, box_menu_top.h);
		// right menu bottom
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(249, 254, 143));
		DRAW_RECT(box_menu_bottom.x_end, box_menu_bottom.y, 1280 - box_menu_bottom.w, box_menu_bottom.h);

		// right menu HR
		DRAW_BOX_BORDER(box_right1, 0x000000, border, 0xffffff);
		// right menu spO2
		DRAW_BOX_BORDER(box_right2, 0x000000, border, 0xffffff);
		// right menu etCO2
		DRAW_BOX_BORDER(box_right3, 0x000000, border, 0xffffff);
		// right menu NIBP
		DRAW_BOX_BORDER(box_right4, 0x000000, border, 0xffffff);


		//buttons
		int x = box_menu_top.x;
		int y = box_menu_top.y_mid;
		int btn_w = 180;
		int btn_h = 50;
		int btn_margin = 50;

		// Record button
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(33, 33, 33));
		EVE_CoCmd_fgColor(s_pHalContext, 0x144344);
		EVE_CoCmd_button(s_pHalContext, box_menu_top.x + btn_w * 1, box_menu_bottom.y + 7, btn_w, btn_h, 30, OPT_FLAT, "RECORD");

		// Stop button
		if (btnStartState == BTN_START_ACTIVE) {
			EVE_CoCmd_fgColor(s_pHalContext, 0xF3B475);
		}
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, TAG(TAG_START_STOP));
		EVE_CoCmd_button(s_pHalContext, box_menu_top.x + btn_w * 2 + btn_margin, box_menu_bottom.y + 7, btn_w, btn_h, 30, 0, btnStartTxt);
		EVE_Cmd_wr32(s_pHalContext, TAG(0));

		// NIBP button
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(33, 33, 33));
		EVE_CoCmd_fgColor(s_pHalContext, 0x144344);
		EVE_CoCmd_button(s_pHalContext, box_menu_top.x + btn_w * 3 + btn_margin * 2, box_menu_bottom.y + 7, btn_w, btn_h, 30, OPT_FLAT, "NIBP");
		
		// EXIT button
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(33, 33, 33));
		EVE_CoCmd_button(s_pHalContext, box_right4.x + (box_right4.w/2 - btn_w/2), box_menu_bottom.y + 7, btn_w, btn_h, 30, OPT_FLAT, "EXIT");

#define ENABLE_SHOW_FPS 0
#if ENABLE_SHOW_FPS
		// Frame per second measurement
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		EVE_CoCmd_text(s_pHalContext, app_window.x + 10, app_window.y_end - 40, 30, OPT_FORMAT, "fps = %d", getFPS());
#endif

		// register big font 32 33 34
		EVE_CoCmd_romFont(s_pHalContext, FONT_32, 32);
		EVE_CoCmd_romFont(s_pHalContext, FONT_33, 33);
		EVE_CoCmd_romFont(s_pHalContext, FONT_34, 34);

		x = box_menu_top.x;
		y = box_menu_top.y_mid;
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
#define ENABLE_FONT_CUSTOM 0
#if ENABLE_FONT_CUSTOM
		EVE_CoCmd_text(s_pHalContext, x + 5, y, font1.handler, OPT_CENTERY, "BED");
		EVE_CoCmd_text(s_pHalContext, x + 155, y, font1.handler, OPT_CENTERY, "no 5");
		EVE_CoCmd_text(s_pHalContext, box_menu_top.x_end - 10, y, font1.handler, OPT_CENTERY | OPT_RIGHTX, hh_mm());
		EVE_CoCmd_text(s_pHalContext, box_menu_top.x_mid, y, font1.handler, OPT_CENTER, dd_mm_yyyy());
#else
		EVE_CoCmd_text(s_pHalContext, x + 5, y, FONT_32, OPT_CENTERY, "BED");
		EVE_CoCmd_text(s_pHalContext, x + 155, y, 31, OPT_CENTERY, "no 5");
		EVE_CoCmd_text(s_pHalContext, box_menu_top.x_end - 10, y, 31, OPT_CENTERY | OPT_RIGHTX, hh_mm());
		EVE_CoCmd_text(s_pHalContext, box_menu_top.x_mid, y, 31, OPT_CENTER, dd_mm_yyyy());
#endif

		// zoom level control
		app_box zoombox= INIT_APP_BOX(box_menu_top.x + btn_w * 4 + btn_margin * 3, box_menu_bottom.y_mid, 100, 30);
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(18*16));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
		EVE_DRAW_AT(zoombox.x, zoombox.y);
		EVE_DRAW_AT(zoombox.x_end, zoombox.y);
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
		EVE_CoCmd_setBitmap(s_pHalContext, zoom_out[0], COMPRESSED_RGBA_ASTC_4x4_KHR, 28, 28);
		EVE_Cmd_wr32(s_pHalContext, TAG(TAG_ZOOM_DOWN));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_DRAW_AT(zoombox.x - zoombox.w / 9, zoombox.y- zoombox.h/3);
		EVE_CoCmd_setBitmap(s_pHalContext, zoom_in[0], COMPRESSED_RGBA_ASTC_4x4_KHR, 28, 28);
		EVE_Cmd_wr32(s_pHalContext, TAG(TAG_ZOOM_UP));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_DRAW_AT(zoombox.x_end - zoombox.w / 9, zoombox.y - zoombox.h / 3);
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		EVE_CoCmd_text(s_pHalContext, zoombox.x_mid, zoombox.y_mid - zoombox.h / 3, font2.handler, OPT_FORMAT | OPT_CENTER, "%d", g_graph_zoom_lv);
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
		EVE_Cmd_wr32(s_pHalContext, TAG(TAG_ZOOM_DOWN));
		DRAW_CIRCLE(zoombox.x, zoombox.y, 40);
		EVE_Cmd_wr32(s_pHalContext, TAG(TAG_ZOOM_UP));
		DRAW_CIRCLE(zoombox.x_end, zoombox.y, 40);
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));

		// Graph title text information
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_CoCmd_text(s_pHalContext, box_ecg.x + box_ecg.w / 100, box_ecg.y + box_ecg.h / 10, font2.handler, 0, "ECG");
		EVE_CoCmd_text(s_pHalContext, box_pth.x + box_pth.w / 100, box_pth.y + box_pth.h / 10, font2.handler, 0, "PLETH");
		EVE_CoCmd_text(s_pHalContext, box_co2.x + box_co2.w / 100, box_co2.y + box_co2.h / 10, font2.handler, 0, "CO2");

		// create random data change
		int time_end_ms = EVE_millis();
		int duration = time_end_ms - time_start_ms;
		if (duration > (200 + app_random(100) - 50))
		{
			if (app_random(10) % 9 == 0)
				val_hr = 66 + app_random(8) - 5;
			if (app_random(10) % 5 == 0)
				val_spo2 = 90 + app_random(12) - 6;
			if (app_random(10) % 5 == 0)
				val_co2 = 22 + app_random(4) - 2;
			if (app_random(10) % 2 == 0)
				val_sys = 156 + app_random(8) - 4;
			if (app_random(10) % 3 == 0)
				val_dias = 93 + app_random(8) - 4;

			time_start_ms = time_end_ms;
		}

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 0));
		// Heart rate
		EVE_CoCmd_text(s_pHalContext, box_right1.x + 5, box_right1.y + 5, font2.handler, 0, "HR");
		EVE_CoCmd_number(s_pHalContext, box_right1.x_mid, box_right1.y_mid, font0.handler, OPT_CENTER, val_hr);
		EVE_CoCmd_text(s_pHalContext, box_right1.x_mid + 40, box_right1.y_mid, font2.handler, OPT_CENTERY, "bpm");

		// SPO2
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 255));
		EVE_CoCmd_text(s_pHalContext, box_right2.x + 5, box_right2.y + 5, font2.handler, 0, "spO2");
		EVE_CoCmd_number(s_pHalContext, box_right2.x_mid, box_right2.y_mid, FONT_33, OPT_CENTER, val_spo2);
		EVE_CoCmd_text(s_pHalContext, box_right2.x_mid + 40, box_right2.y_mid, font2.handler, OPT_CENTERY, "%");

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 0));
		// ETCO2
		EVE_CoCmd_text(s_pHalContext, box_right3.x + 5, box_right3.y + 5, font2.handler, 0, "etCO2");
		EVE_CoCmd_number(s_pHalContext, box_right3.x_mid, box_right3.y_mid, FONT_33, OPT_CENTER, val_co2);
		EVE_CoCmd_text(s_pHalContext, box_right3.x_mid + 40, box_right3.y_mid, font2.handler, OPT_CENTERY, "mmHg");

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		// NIBP
		EVE_CoCmd_text(s_pHalContext, box_right4.x + 5, box_right4.y + 5, font2.handler, 0, "NIBP");
		EVE_CoCmd_number(s_pHalContext, box_right4.x + 2, box_right4.y + 55, FONT_32, 0, val_sys);
		EVE_CoCmd_text(s_pHalContext, box_right4.x_mid+4, box_right4.y_mid, font2.handler, OPT_CENTERX, "mmHg");
		EVE_CoCmd_number(s_pHalContext, box_right4.x_mid + 50, box_right4.y + 55, FONT_32, 0, val_dias);
		EVE_CoCmd_text(s_pHalContext, box_right4.x + 35, box_right4.y_end - 40, font2.handler, 0, "sys");
		EVE_CoCmd_text(s_pHalContext, box_right4.x_end - 70, box_right4.y_end - 40, font2.handler, 0, "dias");

		Display_End(s_pHalContext);

#if ENABLE_SCREENSHOT_CAPTURE
		screenshot_counter++;
		if (screenshot_counter > 1000) {
			take_ddr_screenshot();
		}
#endif

	}
	return 0;
};
#endif