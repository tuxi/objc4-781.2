//
//  ViewController.m
//  iOSDemo
//
//  Created by xiaoyuan on 2020/10/9.
//

#import "ViewController.h"
#import <objc/runtime.h>

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    @autoreleasepool {
        NSObject *obj = [NSObject new];
        __weak typeof(obj) weakObj = obj;
    }
}


@end
