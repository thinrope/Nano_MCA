#!/usr/bin/python

import sys,struct,zlib,os
from optparse import OptionParser

def named(tuple,names):
  return dict(zip(names.split(),tuple))
def consume(fmt,data,names):
  n = struct.calcsize(fmt)
  return named(struct.unpack(fmt,data[:n]),names),data[n:]
def cstring(string):
  return string.split('\0',1)[0]
def compute_crc(data):
  return 0xFFFFFFFF & -zlib.crc32(data) -1

def parse(file,dump_images=False):
  print 'File: "%s"' % file
  data = open(file,'rb').read()
  crc = compute_crc(data[:-4])
  prefix, data = consume('<5sBIB',data,'signature version size targets')
  print '%(signature)s v%(version)d, image size: %(size)d, targets: %(targets)d' % prefix
  for t in range(prefix['targets']):
    tprefix, data  = consume('<6sBI255s2I',data,'signature altsetting named name size elements')
    tprefix['num'] = t
    if tprefix['named']:
      tprefix['name'] = cstring(tprefix['name'])
    else:
      tprefix['name'] = ''
    print '%(signature)s %(num)d, alt setting: %(altsetting)s, name: "%(name)s", size: %(size)d, elements: %(elements)d' % tprefix
    tsize = tprefix['size']
    target, data = data[:tsize], data[tsize:]
    for e in range(tprefix['elements']):
      eprefix, target = consume('<2I',target,'address size')
      eprefix['num'] = e
      print '  %(num)d, address: 0x%(address)08x, size: %(size)d' % eprefix
      esize = eprefix['size']
      image, target = target[:esize], target[esize:]
      if dump_images:
        out = '%s.target%d.image%d.bin' % (file,t,e)
        open(out,'wb').write(image)
        print '    DUMPED IMAGE TO "%s"' % out
    if len(target):
      print "target %d: PARSE ERROR" % t
  suffix = named(struct.unpack('<4H3sBI',data[:16]),'device product vendor dfu ufd len crc')
  print 'usb: %(vendor)04x:%(product)04x, device: 0x%(device)04x, dfu: 0x%(dfu)04x, %(ufd)s, %(len)d, 0x%(crc)08x' % suffix
  if crc != suffix['crc']:
    print "CRC ERROR: computed crc32 is 0x%08x" % crc
  data = data[16:]
  if data:
    print "PARSE ERROR"

def build(file,targets):
  data = ''
  for t,target in enumerate(targets):
    tdata = ''
    for image in target:
      tdata += struct.pack('<2I',image['address'],len(image['data']))+image['data']
    tdata = struct.pack('<6sBI255s2I','Target',0,1,'ST...',len(tdata),len(target)) + tdata
    data += tdata
  data  = struct.pack('<5sBIB','DfuSe',1,len(data)+11,len(targets)) + data
  data += struct.pack('<4H3sB',0,0xdf11,0x0483,0x011a,'UFD',16)
  crc   = compute_crc(data)
  data += struct.pack('<I',crc)
  open(file,'wb').write(data)

if __name__=="__main__":
  usage = """
%prog [-d|--dump] infile.dfu
%prog {-b|--build} address:file.bin [-b address:file.bin ...] outfile.dfu"""
  parser = OptionParser(usage=usage)
  parser.add_option("-b", "--build", action="append", dest="binfiles",
    help="build a DFU file from given BINFILES", metavar="BINFILES")
  parser.add_option("-d", "--dump", action="store_true", dest="dump_images",
    default=False, help="dump contained images to current directory")
  (options, args) = parser.parse_args()

  if options.binfiles and len(args)==1:
    target = []
    for arg in options.binfiles:
      address,binfile = arg.split(':',1)
      try:
        address = int(address,0) & 0xFFFFFFFF
      except ValueError:
        print "Address %s invalid." % address
        sys.exit(1)
      if not os.path.isfile(binfile):
        print "Unreadable file '%s'." % binfile
        sys.exit(1)
      target.append({ 'address': address, 'data': open(binfile,'rb').read() })
    outfile = args[0]
    build(outfile,[target])
  elif len(args)==1:
    infile = args[0]
    if not os.path.isfile(infile):
      print "Unreadable file '%s'." % infile
      sys.exit(1)
    parse(infile, dump_images=options.dump_images)
  else:
    parser.print_help()
    sys.exit(1)
