#import <Cocoa/Cocoa.h>
#include "MacWindowActivator.h"

void WindowActivator::activate() {
    [NSApp activateIgnoringOtherApps:YES];
}
