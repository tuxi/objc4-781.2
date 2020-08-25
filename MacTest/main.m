//
//  main.m
//  MacTest
//
//  Created by xiaoyuan on 2020/8/24.
//

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        Class Person = objc_allocateClassPair(objc_getClass("NSObject"), "Person", 0);
        objc_registerClassPair(Person);
        id p = [[Person alloc]init];
        NSLog(@"%s  %p",class_getName([Person class]), p);
        
        __weak typeof(p) weakP = p;
        __weak typeof(p) weakP1 = p;
        NSLog(@"%p  %p", weakP, p);
        dispatch_sync(dispatch_queue_create(0, 0), ^{
            __strong typeof(weakP) strongP = weakP;
            NSLog(@"%p  %p  %p  %p", weakP, p, weakP1, strongP);
        });
        NSLog(@"%p  %p", weakP, p);
    }
    return 0;
}
