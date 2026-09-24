PROGRAM{
  DECLPROC ?fun_0;
  DECLPROC ?fun_20;
  DECLPROC ?fun_78683;
  DECLPROC ?fun_103289;
  DECLPROC ?fun_106029;
  DECLPROC ?fun_113700;
  DECLPROC ?fun_ref_229cd1a9ad6b3610;
  ?fun_0 PROC:<{
    s0 s1 XCHG
    CTOS
    4 LDU
    s0 s1 XCHG
    1 PUSHINT
    AND
    <{
      s1 s3 XCHG
      3 BLKDROP
      32 PUSHINT
      SDSKIPFIRST
      32 LDU
      s0 s1 XCHG
      395134233 PUSHINT
      EQUAL
      <{
        s0 POP
      }> PUSHCONT
      IFNOTJMP
      64 PUSHINT
      SDSKIPFIRST
      LDGRAMS
      s0 POP
      c4 PUSHCTR
      CTOS
      LDGRAMS
      LDMSGADDR
      LDMSGADDR
      LDMSGADDR
      LDREF
      LDREF
      ENDS
      s5 s6 XCHG2
      SUB
      s0 s5 XCHG
      s3 s4 XCHG_IJ
      s1 s3 s0 XCHG3
      NEWC
      s0 s6 XCHG2
      STGRAMS
      s0 s4 XCHG2
      STSLICER
      ROT
      STSLICER
      s0 s1 XCHG
      STSLICER
      STREF
      STREF
      ENDC
      c4 POPCTR
    }> PUSHCONT
    IFJMP
    LDMSGADDR
    LDMSGADDR
    s1 POP
    LDGRAMS
    s1 POP
    SKIPDICT
    LDGRAMS
    s1 POP
    LDGRAMS
    s0 s1 XCHG
    s1 POP
    0 PUSHINT
    GETORIGINALFWDFEE
    s0 s2 XCHG
    32 LDU
    s0 s1 XCHG
    s0 s1 XCHG
    64 LDU
    s0 s1 XCHG
    s1 s2 XCHG
    c4 PUSHCTR
    CTOS
    LDGRAMS
    LDMSGADDR
    LDMSGADDR
    LDMSGADDR
    LDREF
    LDREF
    ENDS
    s7 PUSH
    1680571655 PUSHINT
    EQUAL
    <{
      s6 POP
      s6 POP
      s9 POP
      s9 POP
      s5 s0 PUSH2
      SDEQ
      s6 s8 XCPU
      SDEQ
      s1 s6 XCHG
      OR
      75 THROWIFNOT
      s0 s3 XCHG
      LDMSGADDR
      s1 PUSH
      REWRITESTDADDR
      s0 POP
      0 EQINT
      333 THROWIFNOT
      LDGRAMS
      LDREF
      ENDS
      s0 PUSH
      CTOS
      32 LDU
      s0 s1 XCHG
      395134233 PUSHINT
      EQUAL
      72 THROWIFNOT
      64 PUSHINT
      SDSKIPFIRST
      LDGRAMS
      LDMSGADDR
      s1 POP
      LDMSGADDR
      s1 POP
      LDGRAMS
      s0 PUSH
      1 PLDU
      <{
        SBITREFS
        1 EQINT
        s0 s1 XCHG
        1 EQINT
        AND
        49 THROWIFNOT
      }> PUSHCONT
      <{
        s0 POP
      }> PUSHCONT
      IFELSE
      s3 s2 s11 PUXC2
      s1 PUSH
      <{
        2 PUSHINT
      }> PUSHCONT
      <{
        1 PUSHINT
      }> PUSHCONT
      IFELSE
      GETPRECOMPILEDGAS
      s0 PUSH
      ISNULL
      <{
        9400 PUSHINT
      }> PUSHCONT
      <{
        s0 PUSH
      }> PUSHCONT
      IFELSE
      s1 PUSH
      ISNULL
      <{
        s1 POP
        10355 PUSHINT
      }> PUSHCONT
      <{
        s0 s1 XCHG
      }> PUSHCONT
      IFELSE
      s2 s3 XCHG2
      MUL
      s1 s3 XCHG
      ADD
      3 PUSHINT
      931 PUSHINT
      0 PUSHINT
      GETFORWARDFEESIMPLE
      ADD
      s0 s2 XCHG
      0 PUSHINT
      GETGASFEE
      s1 s2 XCHG
      ADD
      s0 s1 XCHG
      0 PUSHINT
      GETGASFEE
      ADD
      3 PUSHINT
      1033 PUSHINT
      157680000 PUSHINT
      0 PUSHINT
      GETSTORAGEFEE
      ADD
      GREATER
      48 THROWIFNOT
      s4 PUSH
      ROTREV
      -1 PUSHINT
      1000000000 PUSHINT
      0 PUSHINT
      RAWRESERVE
      MYADDR
      s5 s0 s4 XCHG3
      0 PUSHINT
      s0 s1 s6 PU2XC
      s1 s3 XCHG
      s1 s5 XCHG
      s0 s3 XCHG
      NEWC
      4 STU
      ROT
      STGRAMS
      s0 s1 XCHG
      STSLICER
      s0 s1 XCHG
      STSLICER
      ENDC
      s1 PUSH
      NEWC
      2 STU
      s1 s3 XCHG
      STDICT
      s1 s2 XCHG
      STDICT
      1 STU
      ENDC
      s0 PUSH
      HASHCU
      0 PUSHINT
      4 PUSHINT
      NEWC
      3 STU
      8 STI
      256 STU
      ENDC
      CTOS
      NEWC
      24 PUSHINT
      s0 s1 XCHG
      6 STU
      s0 s1 XCHG
      STSLICER
      ROT
      STGRAMS
      s0 s2 XCHG
      <{
        ROT
        7 PUSHINT
        s0 s3 XCHG2
        108 STU
        STREF
        STREF
      }> PUSHCONT
      <{
        s0 POP
        s0 s1 XCHG
        1 PUSHINT
        ROT
        107 STU
        STREF
      }> PUSHCONT
      IFELSE
      ENDC
      17 PUSHINT
      SENDRAWMSG
      s2 s4 XCHG2
      ADD
      s4 s1 s5 XCHG3
      s0 s3 XCHG
      NEWC
      s0 s6 XCHG2
      STGRAMS
      s0 s4 XCHG2
      STSLICER
      ROT
      STSLICER
      s0 s1 XCHG
      STSLICER
      STREF
      STREF
      ENDC
      c4 POPCTR
    }> PUSHCONT
    IFJMP
    s7 PUSH
    395134233 PUSHINT
    EQUAL
    <{
      s6 s7 XCHG_IJ
      7 BLKDROP
      s3 POP
      s4 s3 s4 XCHG3
      ?fun_ref_229cd1a9ad6b3610 CALLREF
    }> PUSHCONT
    IFJMP
    s11 POP
    s11 POP
    s5 PUSH
    2078119902 PUSHINT
    EQUAL
    <{
      s5 POP
      s7 POP
      s0 s4 XCHG
      LDGRAMS
      LDMSGADDR
      MYADDR
      s2 s0 s11 XC2PU
      0 PUSHINT
      s0 s1 s6 PU2XC
      s1 s3 XCHG
      s1 s5 XCHG
      s0 s3 XCHG
      NEWC
      4 STU
      ROT
      STGRAMS
      s0 s1 XCHG
      STSLICER
      s0 s1 XCHG
      STSLICER
      ENDC
      s1 PUSH
      NEWC
      2 STU
      s1 s3 XCHG
      STDICT
      s1 s2 XCHG
      STDICT
      1 STU
      ENDC
      HASHCU
      0 PUSHINT
      4 PUSHINT
      NEWC
      3 STU
      8 STI
      256 STU
      ENDC
      CTOS
      s0 s7 XCHG2
      SDEQ
      74 THROWIFNOT
      SUB
      s0 s5 XCHG
      s0 s3 XCHG
      s0 s4 XCHG
      s7 s6 XCHG2
      NEWC
      s0 s6 XCHG2
      STGRAMS
      s0 s4 XCHG2
      STSLICER
      ROT
      STSLICER
      s0 s1 XCHG
      STSLICER
      STREF
      STREF
      ENDC
      c4 POPCTR
      LDMSGADDR
      ENDS
      s0 PUSH
      2 PLDU
      0 EQINT
      NOT
      <{
        DROP2
      }> PUSHCONT
      <{
        NEWC
        16 PUSHINT
        s0 s1 XCHG
        6 STU
        s0 s1 XCHG
        STSLICER
        0 PUSHINT
        STGRAMS
        0 PUSHINT
        s0 s1 XCHG
        107 STU
        3576854235 PUSHINT
        s0 s1 XCHG
        32 STU
        s0 s1 XCHG
        s0 s1 XCHG
        64 STU
        ENDC
        66 PUSHINT
        SENDRAWMSG
      }> IFREFELSE
    }> IFJMPREF
    s5 PUSH
    745978227 PUSHINT
    EQUAL
    <{
      s4 s5 XCHG_IJ
      5 BLKDROP
      s3 POP
      s3 POP
      s0 s2 XCHG
      LDMSGADDR
      1 LDI
      s0 s1 XCHG
      s0 s1 XCHG
      ENDS
      <{
        NEWC
        s1 PUSH
        STSLICER
        ENDC
      }> PUSHCONT
      <{
        NULL
      }> PUSHCONT
      IFELSE
      NEWC
      16 PUSHINT
      s0 s1 XCHG
      6 STU
      s0 s4 XCHG2
      STSLICER
      0 PUSHINT
      STGRAMS
      0 PUSHINT
      s0 s1 XCHG
      107 STU
      3513996288 PUSHINT
      s0 s1 XCHG
      32 STU
      ROT
      s0 s1 XCHG
      64 STU
      s1 PUSH
      REWRITESTDADDR
      s0 POP
      0 EQINT
      <{
        MYADDR
        s0 s4 XCHG2
        0 PUSHINT
        s0 s1 s6 PU2XC
        s1 s3 XCHG
        s1 s5 XCHG
        s0 s3 XCHG
        NEWC
        4 STU
        ROT
        STGRAMS
        s0 s1 XCHG
        STSLICER
        s0 s1 XCHG
        STSLICER
        ENDC
        s1 PUSH
        NEWC
        2 STU
        s1 s3 XCHG
        STDICT
        s1 s2 XCHG
        STDICT
        1 STU
        ENDC
        HASHCU
        0 PUSHINT
        4 PUSHINT
        NEWC
        3 STU
        8 STI
        256 STU
        ENDC
        CTOS
        s1 s2 XCHG
        STSLICER
      }> PUSHCONT
      <{
        s1 POP
        1 2 BLKDROP2
        0 PUSHINT
        s0 s1 XCHG
        2 STU
      }> PUSHCONT
      IFELSE
      STDICT
      ENDC
      80 PUSHINT
      SENDRAWMSG
    }> IFJMPREF
    s4 POP
    s4 PUSH
    1694626644 PUSHINT
    EQUAL
    <{
      s0 POP
      s3 POP
      s5 POP
      s3 s1 XCPU
      SDEQ
      73 THROWIFNOT
      s0 s1 XCHG
      LDMSGADDR
      ENDS
      s0 s3 XCHG
      s5 s5 XCHG2
      s0 s4 XCHG
      NEWC
      s0 s6 XCHG2
      STGRAMS
      s0 s4 XCHG2
      STSLICER
      ROT
      STSLICER
      s0 s1 XCHG
      STSLICER
      STREF
      STREF
      ENDC
      c4 POPCTR
    }> PUSHCONT
    IFJMP
    s4 PUSH
    4220051737 PUSHINT
    EQUAL
    <{
      s1 POP
      s3 POP
      s5 POP
      s0 s2 XCHG
      ENDS
      s2 s2 XCPU
      SDEQ
      73 THROWIFNOT
      x{2_} PUSHSLICE
      s3 s5 XCHG_IJ
      s4 s3 s3 XCHG3
      s0 s2 XCHG
      NEWC
      s0 s6 XCHG2
      STGRAMS
      s0 s4 XCHG2
      STSLICER
      ROT
      STSLICER
      s0 s1 XCHG
      STSLICER
      STREF
      STREF
      ENDC
      c4 POPCTR
    }> PUSHCONT
    IFJMP
    s8 POP
    s3 PUSH
    593276754 PUSHINT
    EQUAL
    <{
      s7 POP
      3 BLKDROP
      s0 s3 XCHG
      SDEQ
      73 THROWIFNOT
      s0 s1 XCHG
      LDMSGADDR
      LDGRAMS
      LDREF
      ENDS
      s0 PUSH
      CTOS
      32 LDU
      s0 s1 XCHG
      s0 s1 XCHG
      64 PUSHINT
      SDSKIPFIRST
      s1 PUSH
      260734629 PUSHINT
      EQUAL
      <{
        s5 POP
        s0 PUSH
        1499400124 PUSHINT
        EQUAL
        <{
          s0 POP
          s0 s3 XCHG
          LDGRAMS
          s1 POP
          LDMSGADDR
          s1 POP
          SKIPDICT
          ENDS
          s0 PUSH
          GETPRECOMPILEDGAS
          s0 PUSH
          ISNULL
          <{
            s0 POP
            6159 PUSHINT
          }> PUSHCONT
          IF
          1 PUSHINT
          754 PUSHINT
          0 PUSHINT
          GETFORWARDFEE
          s0 s1 XCHG
          0 PUSHINT
          GETGASFEE
          ADD
          7076 PUSHINT
          0 PUSHINT
          GETGASFEE
          ADD
          GREATER
          48 THROWIFNOT
        }> PUSHCONT
        <{
          4006754003 PUSHINT
          EQUAL
          <{
            s0 s3 XCHG
            4 LDU
            s1 POP
            ENDS
          }> PUSHCONT
          <{
            s3 POP
            72 THROW
          }> PUSHCONT
          IFELSE
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      <{
        s1 POP
        LDGRAMS
        s1 POP
        LDMSGADDR
        s1 POP
        LDMSGADDR
        s1 POP
        SKIPDICT
        LDGRAMS
        s0 PUSH
        1 PLDU
        <{
          SBITREFS
          1 EQINT
          s0 s1 XCHG
          1 EQINT
          AND
          49 THROWIFNOT
        }> PUSHCONT
        <{
          s0 POP
        }> PUSHCONT
        IFELSE
        s2 s2 s6 PUXC2
        s1 PUSH
        <{
          2 PUSHINT
        }> PUSHCONT
        <{
          1 PUSHINT
        }> PUSHCONT
        IFELSE
        GETPRECOMPILEDGAS
        s0 PUSH
        ISNULL
        <{
          9400 PUSHINT
        }> PUSHCONT
        <{
          s0 PUSH
        }> PUSHCONT
        IFELSE
        s1 PUSH
        ISNULL
        <{
          s1 POP
          10355 PUSHINT
        }> PUSHCONT
        <{
          s0 s1 XCHG
        }> PUSHCONT
        IFELSE
        s2 s3 XCHG2
        MUL
        s1 s3 XCHG
        ADD
        3 PUSHINT
        931 PUSHINT
        0 PUSHINT
        GETFORWARDFEESIMPLE
        ADD
        s0 s2 XCHG
        0 PUSHINT
        GETGASFEE
        s1 s2 XCHG
        ADD
        s0 s1 XCHG
        0 PUSHINT
        GETGASFEE
        ADD
        3 PUSHINT
        1033 PUSHINT
        157680000 PUSHINT
        0 PUSHINT
        GETSTORAGEFEE
        ADD
        GREATER
        48 THROWIFNOT
      }> IFREFELSE
      s3 s0 s3 XCHG3
      0 PUSHINT
      1000000000 PUSHINT
      0 PUSHINT
      RAWRESERVE
      MYADDR
      s5 s0 s4 XCHG3
      0 PUSHINT
      s0 s1 s6 PU2XC
      s1 s3 XCHG
      s1 s5 XCHG
      s0 s3 XCHG
      NEWC
      4 STU
      ROT
      STGRAMS
      s0 s1 XCHG
      STSLICER
      s0 s1 XCHG
      STSLICER
      ENDC
      s1 PUSH
      NEWC
      2 STU
      s1 s3 XCHG
      STDICT
      s1 s2 XCHG
      STDICT
      1 STU
      ENDC
      s0 PUSH
      HASHCU
      0 PUSHINT
      4 PUSHINT
      NEWC
      3 STU
      8 STI
      256 STU
      ENDC
      CTOS
      NEWC
      24 PUSHINT
      s0 s1 XCHG
      6 STU
      s0 s1 XCHG
      STSLICER
      ROT
      STGRAMS
      s0 s2 XCHG
      <{
        ROT
        7 PUSHINT
        s0 s3 XCHG2
        108 STU
        STREF
        STREF
      }> PUSHCONT
      <{
        s0 POP
        s0 s1 XCHG
        1 PUSHINT
        ROT
        107 STU
        STREF
      }> PUSHCONT
      IFELSE
      ENDC
      17 PUSHINT
      SENDRAWMSG
    }> IFJMPREF
    s6 POP
    s2 PUSH
    3414567170 PUSHINT
    EQUAL
    <{
      s2 POP
      s3 s4 XCPU
      SDEQ
      73 THROWIFNOT
      NEWC
      ROT
      STSLICER
      ENDC
      s1 s5 XCHG
      s3 s4 XCHG_IJ
      NEWC
      s0 s6 XCHG2
      STGRAMS
      s0 s4 XCHG2
      STSLICER
      ROT
      STSLICER
      s0 s1 XCHG
      STSLICER
      STREF
      STREF
      ENDC
      c4 POPCTR
    }> PUSHCONT
    IFJMP
    DROP2
    s4 POP
    s4 POP
    s2 PUSH
    621336170 PUSHINT
    EQUAL
    <{
      s2 POP
      SDEQ
      73 THROWIFNOT
      LDREF
      LDREF
      ENDS
      s0 s1 XCHG
      c4 POPCTR
      SETCODE
    }> PUSHCONT
    IFJMP
    s2 s3 XCHG_IJ
    3 BLKDROP
    3547469196 PUSHINT
    EQUAL
    IFRET
    16 PUSHPOW2DEC
    THROWANY
  }>
  ?fun_20 PROC:<{
    0 PUSHINT
    NEWC
    8 STU
    s0 s1 XCHG
    STSLICER
    ENDC
    s0 s2 XCHG
    8 PUSHPOW2
    DICTUSETREF
  }>
  ?fun_78683 PROC:<{
    c4 PUSHCTR
    CTOS
    LDGRAMS
    LDMSGADDR
    LDMSGADDR
    LDMSGADDR
    LDREF
    LDREF
    ENDS
    s3 s5 XCHG_IJ
    5 BLKDROP
  }>
  ?fun_103289 PROC:<{
    c4 PUSHCTR
    CTOS
    LDGRAMS
    LDMSGADDR
    LDMSGADDR
    LDMSGADDR
    LDREF
    LDREF
    ENDS
    s1 s5 XCHG
    5 BLKDROP
    MYADDR
    s0 s1 XCHG
    0 PUSHINT
    s0 s1 s6 PU2XC
    s1 s3 XCHG
    s1 s5 XCHG
    s0 s3 XCHG
    NEWC
    4 STU
    ROT
    STGRAMS
    s0 s1 XCHG
    STSLICER
    s0 s1 XCHG
    STSLICER
    ENDC
    s1 PUSH
    NEWC
    2 STU
    s1 s3 XCHG
    STDICT
    s1 s2 XCHG
    STDICT
    1 STU
    ENDC
    HASHCU
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
  }>
  ?fun_106029 PROC:<{
    c4 PUSHCTR
    CTOS
    LDGRAMS
    LDMSGADDR
    LDMSGADDR
    LDMSGADDR
    LDREF
    LDREF
    ENDS
    2 2 BLKDROP2
    -1 PUSHINT
    s0 s1 XCHG
    CTOS
    NULL
    51065135818459385347574250312853146822620586594996463797054414300406918686668 PUSHINT
    ROT
    ?fun_20 CALLDICT
    107878361799212983662495570378745491379550006934010968359181619763835345146430 PUSHINT
    x{36} PUSHSLICE
    ?fun_20 CALLDICT
    0 PUSHINT
    NEWC
    8 STU
    STDICT
    ENDC
    s3 s0 s3 XCHG3
  }>
  ?fun_113700 PROC:<{
    c4 PUSHCTR
    CTOS
    LDGRAMS
    LDMSGADDR
    LDMSGADDR
    LDMSGADDR
    LDREF
    LDREF
    ENDS
  }>
  ?fun_ref_229cd1a9ad6b3610 PROCREF:<{
    c4 PUSHCTR
    CTOS
    LDGRAMS
    LDMSGADDR
    LDMSGADDR
    LDMSGADDR
    LDREF
    LDREF
    ENDS
    s0 s10 XCHG
    LDGRAMS
    s6 s1 XCPU
    SUB
    s0 s6 XCHG
    LDMSGADDR
    LDMSGADDR
    MYADDR
    s3 s7 s3 PU2XC
    0 PUSHINT
    s0 s1 s6 PU2XC
    s1 s3 XCHG
    s1 s5 XCHG
    s0 s3 XCHG
    NEWC
    4 STU
    ROT
    STGRAMS
    s0 s1 XCHG
    STSLICER
    s0 s1 XCHG
    STSLICER
    ENDC
    s1 PUSH
    NEWC
    2 STU
    s1 s3 XCHG
    STDICT
    s1 s2 XCHG
    STDICT
    1 STU
    ENDC
    HASHCU
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
    s0 s13 XCHG2
    SDEQ
    74 THROWIFNOT
    s0 s11 XCHG
    LDGRAMS
    s1 PUSH
    <{
      4 BLKDROP
    }> PUSHCONT
    <{
      NEWC
      1935855772 PUSHINT
      s0 s1 XCHG
      32 STU
      s10 PUSH
      s0 s1 XCHG
      64 STU
      s0 s4 XCHG2
      STGRAMS
      ROT
      STSLICER
      ROT
      STSLICER
      ENDC
      NEWC
      16 PUSHINT
      s0 s1 XCHG
      6 STU
      s4 PUSH
      STSLICER
      ROT
      STGRAMS
      s0 s1 XCHG
      1 PUSHINT
      ROT
      107 STU
      STREF
      ENDC
      17 PUSHINT
      SENDRAWMSG
    }> IFREFELSE
    s8 PUSH
    2 PLDU
    0 EQINT
    NOT
    <{
      s3 s8 XCHG_IJ
      s2 s7 XCHG_IJ
      s5 POP
      s5 POP
      DROP2
    }> PUSHCONT
    <{
      s7 s6 XCHG2
      SUB
      16 PUSHINT
      RAWRESERVE
      NEWC
      16 PUSHINT
      s0 s1 XCHG
      6 STU
      s0 s7 XCHG2
      STSLICER
      0 PUSHINT
      STGRAMS
      0 PUSHINT
      s0 s1 XCHG
      107 STU
      3576854235 PUSHINT
      s0 s1 XCHG
      32 STU
      s0 s3 XCHG2
      s0 s1 XCHG
      64 STU
      ENDC
      130 PUSHINT
      SENDRAWMSG
      s1 s4 XCHG
      s3 s3 s0 XCHG3
    }> IFREFELSE
    s0 s5 XCHG
    NEWC
    s0 s6 XCHG2
    STGRAMS
    s0 s4 XCHG2
    STSLICER
    ROT
    STSLICER
    s0 s1 XCHG
    STSLICER
    STREF
    STREF
    ENDC
    c4 POPCTR
  }>
}END>c