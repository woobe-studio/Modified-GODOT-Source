/**************************************************************************/
/*  godot_view_gesture_recognizer.mm                                      */
/**************************************************************************/


#import "godot_view_gesture_recognizer.h"
#import "godot_view.h"

#include "core/project_settings.h"

// Minimum distance for touches to move to fire
// a delay timer before scheduled time.
// Should be the low enough to not cause issues with dragging
// but big enough to allow click to work.
const CGFloat kGLGestureMovementDistance = 0.5;

@interface GodotViewGestureRecognizer ()

@property(nonatomic, readwrite, assign) NSTimeInterval delayTimeInterval;

@end

@implementation GodotViewGestureRecognizer

- (GodotView *)godotView {
	return (GodotView *)self.view;
}

- (instancetype)init {
	self = [super init];

	self.cancelsTouchesInView = YES;
	self.delaysTouchesBegan = YES;
	self.delaysTouchesEnded = YES;
	self.requiresExclusiveTouchType = NO;

	self.delayTimeInterval = GLOBAL_GET("input_devices/pointing/ios/touch_delay");

	return self;
}

- (void)delayTouches:(NSSet *)touches andEvent:(UIEvent *)event {
	[delayTimer fire];

	delayedTouches = touches;
	delayedEvent = event;

	delayTimer = [NSTimer scheduledTimerWithTimeInterval:self.delayTimeInterval target:self selector:@selector(fireDelayedTouches:) userInfo:nil repeats:NO];
}

- (void)fireDelayedTouches:(id)timer {
	[delayTimer invalidate];
	delayTimer = nil;

	if (delayedTouches) {
		[self.godotView godotTouchesBegan:delayedTouches withEvent:delayedEvent];
	}

	delayedTouches = nil;
	delayedEvent = nil;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	NSSet *cleared = [self copyClearedTouches:touches phase:UITouchPhaseBegan];
	[self delayTouches:cleared andEvent:event];

	[super touchesBegan:touches withEvent:event];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	NSSet *cleared = [self copyClearedTouches:touches phase:UITouchPhaseMoved];

	if (delayTimer) {
		// We should check if movement was significant enough to fire an event
		// for dragging to work correctly.
		for (UITouch *touch in cleared) {
			CGPoint from = [touch locationInView:self.godotView];
			CGPoint to = [touch previousLocationInView:self.godotView];
			CGFloat xDistance = from.x - to.x;
			CGFloat yDistance = from.y - to.y;

			CGFloat distance = sqrt(xDistance * xDistance + yDistance * yDistance);

			// Early exit, since one of touches has moved enough to fire a drag event.
			if (distance > kGLGestureMovementDistance) {
				[delayTimer fire];
				[self.godotView godotTouchesMoved:cleared withEvent:event];
				return;
			}
		}
		return;
	}

	[self.godotView godotTouchesMoved:cleared withEvent:event];

	[super touchesMoved:touches withEvent:event];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	[delayTimer fire];

	NSSet *cleared = [self copyClearedTouches:touches phase:UITouchPhaseEnded];
	[self.godotView godotTouchesEnded:cleared withEvent:event];

	[super touchesEnded:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	[delayTimer fire];
	[self.godotView godotTouchesCancelled:touches withEvent:event];

	[super touchesCancelled:touches withEvent:event];
}

- (NSSet *)copyClearedTouches:(NSSet *)touches phase:(UITouchPhase)phaseToSave {
	NSMutableSet *cleared = [touches mutableCopy];

	for (UITouch *touch in touches) {
		if (touch.view != self.view || touch.phase != phaseToSave) {
			[cleared removeObject:touch];
		}
	}

	return cleared;
}

@end
