/** Galois Network Layer Generic Support -*- C++ -*-
 * @file
 * @section License
 *
 * Galois, a framework to exploit amorphous data-parallelism in irregular
 * programs.
 *
 * Copyright (C) 2012, The University of Texas at Austin. All rights reserved.
 * UNIVERSITY EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING THIS
 * SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR ANY PARTICULAR PURPOSE, NON-INFRINGEMENT AND WARRANTIES OF
 * PERFORMANCE, AND ANY WARRANTY THAT MIGHT OTHERWISE ARISE FROM COURSE OF
 * DEALING OR USAGE OF TRADE.  NO WARRANTY IS EITHER EXPRESS OR IMPLIED WITH
 * RESPECT TO THE USE OF THE SOFTWARE OR DOCUMENTATION. Under no circumstances
 * shall University be liable for incidental, special, indirect, direct or
 * consequential damages or loss of profits, interruption of business, or
 * related expenses which may arise from use of Software or Documentation,
 * including but not limited to those resulting from defects in Software and/or
 * Documentation, or loss or inaccuracy of data of any kind.
 *
 * @author Andrew Lenharth <andrewl@lenharth.org>
 */

#include "Galois/Runtime/Tracer.h"
#include "Galois/Runtime/Barrier.h"
#include "Galois/Runtime/Directory.h"

#include <cassert>

uint32_t Galois::Runtime::NetworkInterface::ID = 0;
uint32_t Galois::Runtime::NetworkInterface::Num = 1;

static bool ourexit = false;
static std::deque<std::pair<Galois::Runtime::recvFuncTy, Galois::Runtime::RecvBuffer>> loopwork;

//!landing pad for worker hosts
static void networkExit() {
  assert(Galois::Runtime::NetworkInterface::Num > 1);
  assert(Galois::Runtime::NetworkInterface::ID > 0);
  ourexit = true;
}

static void loop_pad(Galois::Runtime::RecvBuffer& b) {
  uintptr_t f;
  gDeserialize(b, f);
  loopwork.push_back(std::make_pair((Galois::Runtime::recvFuncTy)f, b));
}

void Galois::Runtime::NetworkInterface::start() {
  getSystemBarrier(); // initialize barrier before anyone might be at it
  getSystemNetworkInterface();
  getSystemDirectory();
  if (NetworkInterface::ID != 0) {
    while (!ourexit) {
      doNetworkWork();
      if (!loopwork.empty()) {
        auto p = loopwork.front();
        loopwork.pop_front();
        p.first(p.second);
      }
    }
    exit(0);
  }
}

void Galois::Runtime::NetworkInterface::terminate() {
  //return if just one host is running
  if (NetworkInterface::Num == 1)
    return;
  assert(NetworkInterface::ID == 0);
  NetworkInterface& net = getSystemNetworkInterface();
  net.broadcastAlt(&networkExit);
  doNetworkWork();
  return;
}

void Galois::Runtime::NetworkInterface::sendLoop(uint32_t dest, recvFuncTy recv, SendBuffer& buf) {
  buf.serialize_header((void*)recv);
  Galois::Runtime::getSystemNetworkInterface().send(dest, &loop_pad, buf);
}

//anchor vtable
Galois::Runtime::NetworkInterface::~NetworkInterface() {}

//RealID -> effective ID for the broadcast tree
static unsigned getEID(unsigned realID, unsigned srcID) {
  return (realID + Galois::Runtime::NetworkInterface::Num - srcID) % Galois::Runtime::NetworkInterface::Num;
}

//Effective id in the broadcast tree -> realID
static unsigned getRID(unsigned eID, unsigned srcID) {
  return (eID + srcID) % Galois::Runtime::NetworkInterface::Num;
}

//forward decl
static void bcastLandingPad(Galois::Runtime::RecvBuffer& buf);

//forward message along tree
static void bcastForward(unsigned source, Galois::Runtime::RecvBuffer& buf) {
  static const int width = 2;

  unsigned eid = getEID(Galois::Runtime::NetworkInterface::ID, source);
  
  for (int i = 0; i < width; ++i) {
    unsigned ndst = eid * width + i + 1;
    if (ndst < Galois::Runtime::NetworkInterface::Num) {
      Galois::Runtime::SendBuffer sbuf;
      Galois::Runtime::gSerialize(sbuf, source, buf);
      Galois::Runtime::getSystemNetworkInterface().send(getRID(ndst, source), &bcastLandingPad, sbuf);
    }
  }
}

//recieve broadcast message over the network
static void bcastLandingPad(Galois::Runtime::RecvBuffer& buf) {
  unsigned source;
  Galois::Runtime::gDeserialize(buf, source);
  Galois::Runtime::trace_bcast_recv(source);
  bcastForward(source, buf);
  //deliver locally
  Galois::Runtime::recvFuncTy recv;
  Galois::Runtime::gDeserialize(buf, recv);
  recv(buf);
}

void Galois::Runtime::NetworkInterface::broadcast(recvFuncTy recv, SendBuffer& buf, bool self) {
  unsigned source = Galois::Runtime::NetworkInterface::ID;
  Galois::Runtime::trace_bcast_recv(source);
  buf.serialize_header((void*)recv);
  Galois::Runtime::RecvBuffer rbuf(std::move(buf));
  bcastForward(source, rbuf);
  if (self)
    recv(rbuf);
}

