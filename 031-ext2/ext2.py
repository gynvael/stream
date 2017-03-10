from struct import *

# https://www.vidarholen.net/contents/junk/inodes.html
EXT2_BAD_INO          = 1
EXT2_ROOT_INO         = 2
EXT4_USR_QUOTA_INO    = 3
EXT4_GRP_QUOTA_INO    = 4
EXT2_BOOT_LOADER_INO  = 5
EXT2_UNDEL_DIR_INO    = 6
EXT2_RESIZE_INO       = 7
EXT2_JOURNAL_INO      = 8
EXT2_EXCLUDE_INO      = 9
EXT4_REPLICA_INO      = 10


EXT2_S_IFSOCK = 0xC000  # socket
EXT2_S_IFLNK  = 0xA000  # symbolic link
EXT2_S_IFREG  = 0x8000  # regular file
EXT2_S_IFBLK  = 0x6000  # block device
EXT2_S_IFDIR  = 0x4000  # directory
EXT2_S_IFCHR  = 0x2000  # character device
EXT2_S_IFIFO  = 0x1000  # fifo

EXT2_FT_UNKNOWN  = 0  # Unknown File Type
EXT2_FT_REG_FILE = 1  # Regular File
EXT2_FT_DIR      = 2  # Directory File
EXT2_FT_CHRDEV   = 3  # Character Device
EXT2_FT_BLKDEV   = 4  # Block Device
EXT2_FT_FIFO     = 5  # Buffer File
EXT2_FT_SOCK     = 6  # Socket File
EXT2_FT_SYMLINK  = 7  # Symbolic Link

FT_TO_STR = dict(
    [(y, x) for x, y in globals().items() 
     if "EXT2_FT_" in x])

# http://www.nongnu.org/ext2-doc/ext2.html#SUPERBLOCK
EXT2_SUPERBLOCK = """
0	4	s_inodes_count
4	4	s_blocks_count
8	4	s_r_blocks_count
12	4	s_free_blocks_count
16	4	s_free_inodes_count
20	4	s_first_data_block
24	4	s_log_block_size
28	4	s_log_frag_size
32	4	s_blocks_per_group
36	4	s_frags_per_group
40	4	s_inodes_per_group
44	4	s_mtime
48	4	s_wtime
52	2	s_mnt_count
54	2	s_max_mnt_count
56	2	s_magic
58	2	s_state
60	2	s_errors
62	2	s_minor_rev_level
64	4	s_lastcheck
68	4	s_checkinterval
72	4	s_creator_os
76	4	s_rev_level
80	2	s_def_resuid
82	2	s_def_resgid
84	4	s_first_ino
88	2	s_inode_size
90	2	s_block_group_nr
92	4	s_feature_compat
96	4	s_feature_incompat
100	4	s_feature_ro_compat
104	16	s_uuid
120	16	s_volume_name
136	64	s_last_mounted
200	4	s_algo_bitmap
"""

EXT2_INODE = """
0	2	i_mode
2	2	i_uid
4	4	i_size
8	4	i_atime
12	4	i_ctime
16	4	i_mtime
20	4	i_dtime
24	2	i_gid
26	2	i_links_count
28	4	i_blocks
32	4	i_flags
36	4	i_osd1
40	15 x 4	i_block
100	4	i_generation
104	4	i_file_acl
108	4	i_dir_acl
112	4	i_faddr
116	12	i_osd2
"""

EXT_BLOCKGROUPDESCRIPTOR = """
0	4	bg_block_bitmap
4	4	bg_inode_bitmap
8	4	bg_inode_table
12	2	bg_free_blocks_count
14	2	bg_free_inodes_count
16	2	bg_used_dirs_count
18	2	bg_pad
20	12	bg_reserved
"""

EXT_DIRECTORY_ENTRY = """
0	4	inode
4	2	rec_len
6	1	name_len
7	1	file_type
"""

class DictWrapper:
  def __init__(self, d):
    self._d = d

  def __getattr__(self, name):
    try:
      return getattr(self._d, name)
    except AttributeError:
      pass

    try:
      return self._d[name]
    except KeyError as e:
      raise AttributeError("'%s' object has no attribute '%s'" % (
        type(self).__name__, e.message
      ))      

def parse_from_description(data, desc):
  obj = {}

  for ln in desc.splitlines():
    ln = ln.strip()
    if not ln:
      continue

    offset, size, name = ln.split('\t')

    ftype = {
        '1': 'B',
        '2': 'H',
        '4': 'I',
        '16': '16s',
        '64': '64s',
        '12': '12s',
        '15 x 4': '15I'
    }[size]

    if size == '15 x 4':
      size = 60

    offset = int(offset)
    size = int(size)

    value = unpack(ftype, data[offset:offset+size])
    if len(value) == 1:
      value = value[0]
    obj[name] = value

  return DictWrapper(obj)
    

class ext2:
  def __init__(self, fname):
    with open(fname, 'rb') as f:
      self.d = bytearray(f.read())
    self.parse()

  def parse(self):
    self.superblock = parse_from_description(
        self.d[1024:1024+1024],
        EXT2_SUPERBLOCK)

    self.blocksize = 1024 << self.superblock.s_log_block_size
    self.blocks_per_group = self.superblock.s_blocks_per_group
    self.inodes_per_group = self.superblock.s_inodes_per_group
    self.inode_size = self.superblock.s_inode_size
    self.first_data_block = self.superblock.s_first_data_block
    print "Inodes per group:", self.inodes_per_group
    print "Blocks per group:", self.blocks_per_group 
    #print self.superblock

  def block_offset(self, group, block_in_group):
    return (self.first_data_block +
            group * self.blocks_per_group + 
            block_in_group) * self.blocksize

  def get_block(self, group, block_in_group, number_of_blocks=1):
    return self.d[
        self.block_offset(group, block_in_group):
        self.block_offset(group, block_in_group + number_of_blocks)
    ]

  def get_abs_block(self, block, number_of_blocks=1):
    return self.d[
        block * self.blocksize:
        (block + number_of_blocks) * self.blocksize]

  def size_to_blocks(self, size_in_bytes):
    sz = size_in_bytes + self.blocksize - 1
    return sz / self.blocksize

  def list_inodes(self):
    k = -1
    while True:
      k += 1

      print ""
      print "---------------------------------- Block Group", k
   
      group_offset = self.block_offset(k, 0)

      if group_offset > len(self.d): # XXX
        break

      group_desc = parse_from_description(
          self.get_block(k, 1), EXT_BLOCKGROUPDESCRIPTOR)
      #print group_desc

      inode_bitmap = self.get_block(k, group_desc.bg_inode_bitmap - 1)
      inode_table = self.get_block(
          k, group_desc.bg_inode_table - 1,
          self.size_to_blocks(self.inode_size * self.inodes_per_group))

      #print str(inode_bitmap[:self.inodes_per_group / 8]).encode("hex")

      for i in range(0, self.inodes_per_group):
        idx_bit = i % 8
        idx_byte = i / 8
        if (inode_bitmap[idx_byte] & (1 << idx_bit)) == 0:
          continue

        inode_nr = i + 1 + k * self.inodes_per_group

        # Skip special inodes.
        if inode_nr in [
            EXT2_BAD_INO,
            EXT4_USR_QUOTA_INO,
            EXT4_GRP_QUOTA_INO,
            EXT2_BOOT_LOADER_INO,
            EXT2_UNDEL_DIR_INO,
            EXT2_RESIZE_INO,
            EXT2_JOURNAL_INO,
            EXT2_EXCLUDE_INO,
            EXT4_REPLICA_INO,
            ]:
          continue

        inode = parse_from_description(
            inode_table[i * self.inode_size:(i + 1) * self.inode_size],
            EXT2_INODE)

        if (inode.i_mode & 0xf000) == 0:
          continue

        if (inode.i_mode & EXT2_S_IFLNK) == EXT2_S_IFLNK:
          print "\n--- Group %i, inode %i exists" % (k, inode_nr)
          lnk = pack("15I", *inode.i_block).replace("\0", "")
          print "Symbolic Link:", lnk
          continue

        if (inode.i_mode & EXT2_S_IFDIR) == EXT2_S_IFDIR:
          print "\n--- Group %i, inode %i exists" % (k, inode_nr)
          print "Directory:"

          # XXX: Not supporting indirect blocks!
          for blkid in inode.i_block[:12]:
            dir_block = self.get_abs_block(blkid)
            p = 0

            while p < self.blocksize:
              entry = parse_from_description(
                  dir_block[p:p + 8], EXT_DIRECTORY_ENTRY)
              if entry.inode == 0:
                break

              name = dir_block[p + 8:p + 8 + entry.name_len]             

              print " [ %5i : %-8s ] %s" % (
                  entry.inode,
                  FT_TO_STR[entry.file_type][8:],
                  name)

              p = p + entry.rec_len


          continue

        print "\n--- Group %i, inode %i exists" % (k, inode_nr)
        print "File content (hexii):"

        # XXX: Not supporting indirect blocks!
        data_left = inode.i_size
        data = []
        for blkid in inode.i_block[:12]:
          if data_left == 0:
            break

          if blkid == 0:
            break
          
          block = self.get_abs_block(blkid)
          data_in_block = min(data_left, self.blocksize)
          data.append(str(block[:data_in_block]))

          data_left -= data_in_block

        data = ''.join(data)

        # Dump in HexII representation.
        for i, ch in enumerate(data):
          ch = ord(ch)
          if ch == 0:
            print "  ",
          elif ch not in range(32, 127):
            print "%.2x" % ch,
          else:
            print ".%c" % ch,

          if (i + 1) % 16 == 0:
            print
        print ""

      
      #break

    

    

fs = ext2("partext2.img")
fs.list_inodes()



