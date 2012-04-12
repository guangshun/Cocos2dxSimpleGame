//
//  Cocos2dxSimpleGameAppController.h
//  Cocos2dxSimpleGame
//
//  Created by yong liu on 4/12/12.
//  Copyright guangshun 2012. All rights reserved.
//

@class RootViewController;

@interface AppController : NSObject <UIAccelerometerDelegate, UIAlertViewDelegate, UITextFieldDelegate,UIApplicationDelegate> {
    UIWindow *window;
    RootViewController	*viewController;
}

@end

