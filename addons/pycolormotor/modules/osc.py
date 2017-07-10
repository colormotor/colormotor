from threading import *
import time
import liblo

class OSCClient:
    def __init__(self):
        self.initialized = False
        self.host = 'localhost'
        self.port = 7777
        self.target = None #Typed(liblo.Address)

    def init(self, host='localhost', port=7777):
        print "Initializing OSC client on port " + str(port)
        self.host = host
        self.port = port

        try:
            self.target = liblo.Address(host, port)
            self.initialized = True
        except liblo.AddressError, err:
            print "Could not initialize OSC client"
            print str(err)
            
    # todo handle multiple ports!
    def send(self,addr,stuff):
        if not self.initialized:
            print "osc: Client not initialized!"
            return False


        if addr[0] != '/':
            addr = '/'+addr

        #print "Sending " + addr + ' ' + str(stuff)
        if type(stuff) == str:
            print stuff
            liblo.send(self.target, addr, stuff)
        else:
            print stuff
            liblo.send(self.target, addr, *stuff)


class ServerThread(Thread):
    def __init__(self, server):
        Thread.__init__(self)
        self.alive = True
        self.server = server
        print 'initializing server thread'

    def run(self):
        no_osc = False
        while self.alive:
            if self.server.running:
                if self.server.osc:
                    self.server.osc.recv(10)
                else:
                    if no_osc == False:
                        print 'No osc'
                        no_osc = True
                    self.server.must_restart = True
                    pass
                
            #time.sleep(0.5)
        print 'exiting thread'

class OSCServer:
    def __init__(self):
        print "Should init"
        self.running = False
        self.osc = None
        self.port = 9999
        self.cazzo = "DDDD"
        self.thread = None
        self.server_changed_callback = None
        self.callback = None
        self.must_restart = False

    def init(self, port=9999):
        print self.cazzo
        if self.thread:
            self.stop()

        if self.osc:
            self.osc.free()

        print "Initializing OSC server on port " + str(port)
        time.sleep(0.1)
        self.port = port
        try:
            self.osc = liblo.Server(port)
            self.osc.add_method(None, None, self.recv)
        
            if self.running:
                self.start()

            if self.server_changed_callback:
                self.server_changed_callback()

        except liblo.ServerError, err:
            print 'Could not initialize server'
            print err
        print "Initialized osc"
        print self.osc

    def set_server_changed_callback( self, callback ):
        self.server_changed_callback = callback

    def set_callback( self, callback ):
        print "Setting callback to"
        print callback
        self.callback = callback

    def recv(self, path, args, types, src):
        # # loop through arguments and print them
        # for a, t in zip(args, types):
        #     print a,t,type(a)
        if self.callback:
            self.callback(path,args,types,src)
        # try: 
        #     if self.callback:
        #         self.callback(path,args,types,src)
        # except:
        #     print "Failed recv"
    
    def update(self):
        if self.must_restart:
            print "restarting OSC server"
            self.must_restart = False
            self.init(port=self.port)

    def start(self):
        print "Running OSC server"
        if self.thread:
            self.stop()
        print self.osc
        self.thread = ServerThread(self)
        self.running = True
        self.thread.start()
        

    def stop(self):
        self.running = False
        if self.thread:
            print "Stopping OSC server"
            self.thread.alive = False
            time.sleep(0.1)
            self.thread.join()
            self.thread = None

    def close(self):
        self.stop()
        print "Stopping osc"
        if self.osc:
            self.osc.free()
        self.osc = None
        print "Stopped osc"
        
client = OSCClient()
server = OSCServer()
testval = [20]

def start_server():
    server.start()

def stop_server():
    server.stop()

def close_server():
    server.close()
    
def init_client(port):
    client.init(port=port)

def init_server(port):
    server.init(port=port)

def osc_server_changed():
    print 'server changed'
    #client.send('/taki','bounds')

class Dummy:
    def recv(self, path, args, types, src):
        print "Received " + path + str(args)
    
if __name__=="__main__":
    init_client(9999)
    init_server(7777)

    d = Dummy()
    server.set_callback(d.recv)
    server.set_server_changed_callback(osc_server_changed)
    start_server()
    
    try:
        while True:
            time.sleep(0.005)
    except KeyboardInterrupt:
        close_server()
        print "Bye"
