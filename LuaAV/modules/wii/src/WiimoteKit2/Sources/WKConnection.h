//
//  WKConnection.h
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 12/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

@class IOBluetoothDevice, IOBluetoothL2CAPChannel;
@interface WKConnection : NSObject {
@private
	id wk_delegate;
	IOBluetoothDevice *wk_device;
	IOBluetoothL2CAPChannel *wk_control;
	IOBluetoothL2CAPChannel *wk_interupt;
	
	/* failure support */
	BOOL wk_resend;
	uint8_t wk_status;
	NSMutableArray *wk_backup;
	NSMutableArray *wk_pending;
	
	/* statistics */
	uint16_t wk_retry;
	//uint16_t wk_waiting;
}

- (id)initWithDevice:(IOBluetoothDevice *)aDevice;

- (id)delegate;
- (void)setDelegate:(id)aDelegate;

- (IOReturn)connect;
- (IOReturn)close;

- (IOBluetoothDevice *)device;

/* async send */
- (IOReturn)sendData:(const uint8_t *)data length:(size_t)length context:(void *)ctxt;

@end

@interface NSObject (WKConnectionDelegate)

- (void)connectionDidOpen:(WKConnection *)aConnection;
- (void)connectionDidClose:(WKConnection *)aConnection;
- (void)connection:(WKConnection *)aConnection didReceiveData:(uint8_t *)data length:(size_t)length;
- (void)connection:(WKConnection *)aConnection didSendData:(void *)ctxt error:(IOReturn)status;

@end

