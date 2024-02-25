#include <CoreGraphics/CoreGraphics.h>
#include <objc/message.h>

static id (*msg)(id, SEL) = (id (*)(id, SEL)) objc_msgSend;
static id (*msg4)(id, SEL, id, id) = (id (*)(id, SEL, id, id)) objc_msgSend;
static id (*msg5)(id, SEL, CGRect, bool, id) = (id (*)(id, SEL, CGRect, bool, id)) objc_msgSend;

template <typename _Type>
static inline id msg_with_type(id self, SEL _cmd, _Type type) {
	id(*msg)(id, SEL, _Type) = (id (*)(id, SEL, _Type)) objc_msgSend;
	return msg(self, _cmd, type);
}

static IMP original_IMP;

static void cassia_VDL(id self, SEL _cmd) {
	original_IMP();

	id _settings = msg_with_type<int>(
		(id) objc_getClass("_UIBackdropViewSettings"),
		sel_getUid("settingsForStyle:"),
		2
	);

	id _backdrop_view = msg5(
		msg((id) objc_getClass("_UIBackdropView"), sel_getUid("alloc")),
		sel_getUid("initWithFrame:autosizesToFitSuperview:settings:"),
		CGRectZero,
		true,
		_settings
	);
	msg_with_type<CGFloat>(_backdrop_view, sel_getUid("setAlpha:"), 0.85);
	msg4(msg(self, sel_getUid("view")), sel_getUid("insertSubview:atIndex:"), _backdrop_view, 0);

}

__attribute__((constructor)) static void init(void) {

	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		SEL orig_SEL = sel_getUid("viewDidLoad");
		Method orig_VDL = class_getInstanceMethod(objc_getClass("SBHomeScreenViewController"), orig_SEL);
		original_IMP = method_setImplementation(orig_VDL, (IMP)cassia_VDL);
	});

}
