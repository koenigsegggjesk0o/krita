PROGRAM{
  DECLPROC ?fun_0;
  DECLPROC ?fun_5;
  DECLPROC ?fun_6;
  DECLPROC ?fun_12;
  DECLPROC ?fun_18;
  DECLPROC ?fun_45;
  DECLPROC ?fun_53;
  DECLPROC ?fun_64;
  DECLPROC ?fun_70;
  DECLPROC ?fun_72;
  DECLPROC ?fun_73;
  DECLPROC ?fun_74;
  DECLPROC ?fun_75;
  DECLPROC ?fun_76;
  DECLPROC ?fun_78;
  DECLPROC ?fun_88;
  DECLPROC ?fun_90;
  DECLPROC ?fun_95;
  DECLPROC ?fun_112;
  DECLPROC ?fun_127;
  DECLPROC ?fun_140;
  DECLPROC ?fun_141;
  DECLPROC ?fun_142;
  DECLPROC ?fun_143;
  DECLPROC ?fun_144;
  DECLPROC ?fun_145;
  DECLPROC ?fun_146;
  DECLPROC ?fun_147;
  DECLPROC ?fun_152;
  DECLPROC ?fun_153;
  DECLPROC ?fun_154;
  DECLPROC ?fun_155;
  DECLPROC ?fun_156;
  DECLPROC ?fun_157;
  DECLPROC ?fun_66977;
  DECLPROC ?fun_67678;
  DECLPROC ?fun_68118;
  DECLPROC ?fun_68315;
  DECLPROC ?fun_68648;
  DECLPROC ?fun_68968;
  DECLPROC ?fun_71979;
  DECLPROC ?fun_72147;
  DECLPROC ?fun_72361;
  DECLPROC ?fun_72737;
  DECLPROC ?fun_73824;
  DECLPROC ?fun_77514;
  DECLPROC ?fun_77798;
  DECLPROC ?fun_78847;
  DECLPROC ?fun_78898;
  DECLPROC ?fun_79569;
  DECLPROC ?fun_80971;
  DECLPROC ?fun_81463;
  DECLPROC ?fun_81750;
  DECLPROC ?fun_82580;
  DECLPROC ?fun_82987;
  DECLPROC ?fun_83291;
  DECLPROC ?fun_85239;
  DECLPROC ?fun_87801;
  DECLPROC ?fun_88345;
  DECLPROC ?fun_88892;
  DECLPROC ?fun_90518;
  DECLPROC ?fun_92829;
  DECLPROC ?fun_93365;
  DECLPROC ?fun_93729;
  DECLPROC ?fun_96161;
  DECLPROC ?fun_96546;
  DECLPROC ?fun_96786;
  DECLPROC ?fun_97013;
  DECLPROC ?fun_99093;
  DECLPROC ?fun_99339;
  DECLPROC ?fun_100035;
  DECLPROC ?fun_100065;
  DECLPROC ?fun_101620;
  DECLPROC ?fun_102394;
  DECLPROC ?fun_102611;
  DECLPROC ?fun_102705;
  DECLPROC ?fun_104233;
  DECLPROC ?fun_105830;
  DECLPROC ?fun_107506;
  DECLPROC ?fun_108308;
  DECLPROC ?fun_110883;
  DECLPROC ?fun_111335;
  DECLPROC ?fun_111835;
  DECLPROC ?fun_111937;
  DECLPROC ?fun_112653;
  DECLPROC ?fun_113356;
  DECLPROC ?fun_114659;
  DECLPROC ?fun_116380;
  DECLPROC ?fun_116476;
  DECLPROC ?fun_117250;
  DECLPROC ?fun_117938;
  DECLPROC ?fun_119021;
  DECLPROC ?fun_119249;
  DECLPROC ?fun_120400;
  DECLPROC ?fun_121426;
  DECLPROC ?fun_121680;
  DECLPROC ?fun_124063;
  DECLPROC ?fun_125767;
  DECLPROC ?fun_126275;
  DECLPROC ?fun_126862;
  DECLPROC ?fun_128150;
  DECLPROC ?fun_128730;
  DECLPROC ?fun_129371;
  DECLPROC ?fun_129818;
  DECLPROC ?fun_130966;
  DECLPROC ?fun_ref_29ce7f6238ca878a;
  DECLPROC ?fun_ref_2ec803d30dedfe82;
  DECLPROC ?fun_ref_7a8d9a5395189798;
  DECLPROC ?fun_ref_8de9528bf3b42745;
  DECLPROC ?fun_ref_99efc4fa43084414;
  DECLPROC ?fun_ref_aabe76dab8bd6f6b;
  DECLPROC ?fun_ref_cc740b3c607066d4;
  DECLPROC ?fun_ref_ec59f05d6c9ac356;
  ?fun_0 PROC:<{
    s3 s1 BLKSWAP
    s3 PUSH
    ?fun_ref_7a8d9a5395189798 CALLREF
    1 GETGLOB
    0 INDEX
    <{
      s0 POP
    }> PUSHCONT
    <{
      125767 PUSHINT
      c3 PUSHCTR
      EXECUTE
      0 PUSHINT
      67678 PUSHINT
      c3 PUSHCTR
      EXECUTE
      1 PUSHINT
      117250 PUSHINT
      c3 PUSHCTR
      EXECUTE
      0 EQINT
      <{
        ?fun_143 CALLDICT
      }> PUSHCONT
      IF
      SEMPTY
      IFRET
      1 GETGLOB
      3 INDEX
      1 GETGLOB
      11 INDEX
      DUP2
      <{
        c2 SAVE
        SAMEALTSAVE
        s1 PUSH
        3812333683 PUSHINT
        EQUAL
        <{
          DROP2
        }> PUSHCONT
        <{
          s1 PUSH
          4133284232 PUSHINT
          EQUAL
          <{
            DROP2
            ?fun_5 CALLDICT
          }> PUSHCONT
          <{
            s1 PUSH
            0 EQINT
            <{
              DROP2
              RETALT
            }> PUSHCONT
            IFJMP
            ?fun_6 CALLDICT
          }> PUSHCONT
          IFELSE
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      EXECUTE
      s1 PUSH
      3812333683 PUSHINT
      EQUAL
      <{
        262 THROW
      }> PUSHCONT
      IF
      s1 PUSH
      4133284232 PUSHINT
      EQUAL
      <{
        s1 POP
        ?fun_140 CALLDICT
        125767 PUSHINT
        c3 PUSHCTR
        EXECUTE
        0 PUSHINT
        67678 PUSHINT
        c3 PUSHCTR
        EXECUTE
        2 PUSHINT
        117250 PUSHINT
        c3 PUSHCTR
        EXECUTE
        <{
          s0 POP
          ?fun_53 CALLDICT
        }> PUSHCONT
        <{
          ?fun_144 CALLDICT
          0 PUSHINT
          s2 s0 PUSH2
          67678 PUSHINT
          c3 PUSHCTR
          EXECUTE
          2 PUSHINT
          -1 PUSHINT
          97013 PUSHINT
          c3 PUSHCTR
          EXECUTE
          s2 s3 XCHG_IJ
          97013 PUSHINT
          c3 PUSHCTR
          EXECUTE
          ?fun_18 CALLDICT
        }> PUSHCONT
        IFELSE
      }> <{
        s1 PUSH
        0 EQINT
        <{
          DROP2
          0 PUSHINT
          1 TUPLE
        }> PUSHCONT
        <{
          125767 PUSHINT
          c3 PUSHCTR
          EXECUTE
          0 PUSHINT
          67678 PUSHINT
          c3 PUSHCTR
          EXECUTE
          2 PUSHINT
          117250 PUSHINT
          c3 PUSHCTR
          EXECUTE
          1273 THROWIFNOT
          <{
            c2 SAVE
            SAMEALTSAVE
            s1 PUSH
            1935855772 PUSHINT
            EQUAL
            <{
              s1 POP
              111835 PUSHINT
              c3 PUSHCTR
              EXECUTE
            }> PUSHCONT
            <{
              s1 PUSH
              2516391687 PUSHINT
              EQUAL
              <{
                s1 POP
                ?fun_153 CALLDICT
                RETALT
              }> PUSHCONT
              IFJMP
              s1 PUSH
              3046071488 PUSHINT
              EQUAL
              <{
                s1 POP
                ?fun_152 CALLDICT
                RETALT
              }> PUSHCONT
              IFJMP
              s1 PUSH
              3249648598 PUSHINT
              EQUAL
              <{
                s1 POP
                ?fun_154 CALLDICT
                RETALT
              }> PUSHCONT
              IFJMP
              s1 PUSH
              795515072 PUSHINT
              EQUAL
              <{
                s1 POP
                ?fun_155 CALLDICT
                RETALT
              }> PUSHCONT
              IFJMP
              s1 PUSH
              372288120 PUSHINT
              EQUAL
              <{
                s1 POP
                ?fun_156 CALLDICT
                RETALT
              }> PUSHCONT
              IFJMP
              s1 PUSH
              1415666607 PUSHINT
              EQUAL
              <{
                s1 POP
                ?fun_157 CALLDICT
                RETALT
              }> PUSHCONT
              IFJMP
              s1 PUSH
              2792556744 PUSHINT
              EQUAL
              <{
                s1 POP
                ?fun_146 CALLDICT
                RETALT
              }> PUSHCONT
              IFJMP
              s1 PUSH
              3072442064 PUSHINT
              EQUAL
              <{
                s1 POP
                ?fun_145 CALLDICT
                RETALT
              }> PUSHCONT
              IFJMP
              s0 s1 XCHG
              2639337746 PUSHINT
              EQUAL
              <{
                ?fun_147 CALLDICT
                RETALT
              }> PUSHCONT
              IFJMP
              s0 POP
              261 THROW
              0 PUSHINT
              1 TUPLE
            }> IFELSEREF
          }> PUSHCONT
          EXECUTE
        }> PUSHCONT
        IFELSE
      }> IFREFELSEREF
      s0 PUSH
      0 INDEX
      1 GETGLOB
      5 INDEX
      12 GETPARAM
      SUB
      1 GETGLOB
      6 INDEX
      1 GETGLOB
      10 INDEX
      SUB
      SUB
      s0 s1 PUSH2
      GEQ
      37 THROWIFNOT
      s0 s1 XCHG
      SUB
      0 PUSHINT
      RAWRESERVE
      -1 PUSHINT
      1 PUSHINT
      s2 PUSH
      TLEN
      <{
        DUP2
        LESS
      }> PUSHCONT
      <{
        s3 s1 PUSH2
        INDEXVAR
        s0 PUSH
        0 INDEX
        s0 s3 XCHG
        INC
        s3 s3 XCHG2
        <{
          c2 SAVE
          SAMEALTSAVE
          s1 PUSH
          435778055796 PUSHINT
          EQUAL
          <{
            s1 POP
            ?fun_127 CALLDICT
          }> PUSHCONT
          <{
            s1 PUSH
            459904164859953153141868 PUSHINT
            EQUAL
            <{
              s1 POP
              ?fun_112 CALLDICT
              RETALT
            }> PUSHCONT
            IFJMP
            s1 PUSH
            32195312204475500 PUSHINT
            EQUAL
            <{
              s1 POP
              ?fun_64 CALLDICT
              RETALT
            }> PUSHCONT
            IFJMP
            s0 s1 XCHG
            544943221246095313366894 PUSHINT
            EQUAL
            <{
              ?fun_70 CALLDICT
              RETALT
            }> PUSHCONT
            IFJMP
            s0 POP
            263 THROW
            0 PUSHINT
          }> PUSHCONT
          IFELSE
        }> PUSHCONT
        EXECUTE
        s1 s3 XCHG
        AND
        s0 s2 XCHG
      }> PUSHCONT
      WHILE
      s2 s3 XCHG_IJ
      3 BLKDROP
      <{
        x{} PUSHREF
        1 PUSHINT
        16 PUSHINT
        NEWC
        6 STU
        1 GETGLOB
        9 INDEX
        0 PUSHINT
        4 PUSHINT
        NEWC
        3 STU
        8 STI
        256 STU
        ENDC
        CTOS
        STSLICER
        0 PUSHINT
        STGRAMS
        107 STU
        STREF
        ENDC
        7 PUSHPOW2
        SENDRAWMSG
      }> PUSHCONT
      IF
    }> IFELSEREF
  }>
  ?fun_5 PROC:<{
    125767 PUSHINT
    c3 PUSHCTR
    EXECUTE
    0 PUSHINT
    67678 PUSHINT
    c3 PUSHCTR
    EXECUTE
    ?fun_45 CALLDICT
    s0 s1 XCHG
    0 PUSHINT
    93365 PUSHINT
    c3 PUSHCTR
    EXECUTE
    EQUAL
    492 THROWIFNOT
  }>
  ?fun_6 PROC:<{
    s0 POP
    s0 PUSH
    1935855772 PUSHINT
    EQUAL
    <{
      s0 POP
      ?fun_142 CALLDICT
    }> PUSHCONT
    <{
      s0 PUSH
      2516391687 PUSHINT
      EQUAL
      s1 PUSH
      3046071488 PUSHINT
      EQUAL
      OR
      s1 PUSH
      3249648598 PUSHINT
      EQUAL
      OR
      s1 PUSH
      795515072 PUSHINT
      EQUAL
      OR
      s1 PUSH
      372288120 PUSHINT
      EQUAL
      OR
      s1 PUSH
      1415666607 PUSHINT
      EQUAL
      OR
      s1 PUSH
      3072442064 PUSHINT
      EQUAL
      OR
      s1 PUSH
      2639337746 PUSHINT
      EQUAL
      OR
      <{
        s0 POP
        125767 PUSHINT
        c3 PUSHCTR
        EXECUTE
        0 PUSHINT
        67678 PUSHINT
        c3 PUSHCTR
        EXECUTE
        ?fun_45 CALLDICT
        s0 s1 XCHG
        0 PUSHINT
        93365 PUSHINT
        c3 PUSHCTR
        EXECUTE
        EQUAL
        492 THROWIFNOT
      }> PUSHCONT
      <{
        2792556744 PUSHINT
        EQUAL
        <{
          ?fun_141 CALLDICT
        }> PUSHCONT
        <{
          261 THROW
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      IFELSE
    }> IFELSEREF
  }>
  ?fun_12 PROC:<{
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    CTOS
    s1 PUSH
    SBITREFS
    s2 PUSH
    SBITREFS
    s3 s3 XCHG2
    NEQ
    s1 s4 PUXC
    NEQ
    s1 s2 XCHG
    OR
    <{
      3 BLKDROP
      0 PUSHINT
    }> PUSHCONT
    <{
      s0 PUSH
      0 EQINT
      <{
        3 BLKDROP
        -1 PUSHINT
      }> PUSHCONT
      IFJMP
      -1 PUSHINT
      0 PUSHINT
      <{
        s2 PUSH
        0 GTINT
      }> PUSHCONT
      <{
        s4 s0 PUSH2
        PLDREFVAR
        s4 s1 PUSH2
        PLDREFVAR
        ?fun_12 CALLDICT
        s1 s2 XCHG
        AND
        s0 s1 XCHG
      }> PUSHCONT
      WHILE
      s1 s4 XCHG
      4 BLKDROP
    }> PUSHCONT
    IFELSE
  }>
  ?fun_18 PROC:<{
    c4 POPCTR
  }>
  ?fun_45 PROC:<{
    1 GETGLOB
    1 INDEX
  }>
  ?fun_53 PROC:<{
    0 PUSHINT
    1 TUPLE
  }>
  ?fun_64 PROC:<{
    s0 PUSH
    1 INDEX
    s0 s1 XCHG
    2 INDEX
    NULL
    0 PUSHINT
    24 PUSHINT
    NEWC
    6 STU
    s0 s4 XCHG
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
    s1 s4 XCHG
    STSLICER
    0 PUSHINT
    STGRAMS
    s1 PUSH
    ISNULL
    <{
      1 PUSHINT
      s2 POP
      107 STU
    }> PUSHCONT
    <{
      7 PUSHINT
      s0 s1 XCHG
      108 STU
      STREF
    }> PUSHCONT
    IFELSE
    STREF
    ENDC
    7 PUSHPOW2
    ROT
    OR
    SENDRAWMSG
    0 PUSHINT
  }>
  ?fun_70 PROC:<{
    s0 PUSH
    1 INDEX
    s1 PUSH
    2 INDEX
    s2 PUSH
    3 INDEX
    s0 s3 XCHG
    4 INDEX
    s1 s3 s0 XCHG3
    NULL
    NULL
    260734629 PUSHINT
    1 GETGLOB
    4 INDEX
    s0 s1 XCHG
    NEWC
    32 STU
    64 STU
    s0 s4 XCHG2
    STGRAMS
    s0 s4 XCHG
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
    s1 s4 XCHG
    STSLICER
    s0 s1 XCHG
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
    STSLICER
    STDICT
    0 PUSHINT
    STGRAMS
    STDICT
    ENDC
    NULL
    0 PUSHINT
    24 PUSHINT
    NEWC
    6 STU
    s0 s4 XCHG
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
    s1 s4 XCHG
    STSLICER
    0 PUSHINT
    STGRAMS
    s1 PUSH
    ISNULL
    <{
      1 PUSHINT
      s2 POP
      107 STU
    }> PUSHCONT
    <{
      7 PUSHINT
      s0 s1 XCHG
      108 STU
      STREF
    }> PUSHCONT
    IFELSE
    STREF
    ENDC
    7 PUSHPOW2
    ROT
    OR
    SENDRAWMSG
    0 PUSHINT
  }>
  ?fun_72 PROC:<{
    s1 PUSH
    SBITS
    s1 PUSH
    SBITS
    ADD
    32385 PUSHINT
    GREATER
    <{
      x{43616E6E6F7420636F6E636174656E6174653A20737472696E6720746F6F206C6F6E67} PUSHSLICE_LONG
      s0 PUSH
      1 4 DEBUG
      s0 POP
      HASHSU
      11 PUSHPOW2DEC
      AND
      THROWANY
    }> PUSHCONT
    IF
    NEWC
    ROT
    STSLICER
    s0 s1 XCHG
    STSLICER
    ENDC
    CTOS
  }>
  ?fun_73 PROC:<{
    ?fun_72 CALLDICT
  }>
  ?fun_74 PROC:<{
    x{} PUSHSLICE
    s1 PUSH
    0 LESSINT
    <{
      s0 s2 XCHG
      x{2D} PUSHSLICE
      ?fun_72 CALLDICT
      s0 s1 XCHG
      NEGATE
      ROT
    }> PUSHCONT
    IF
    s1 PUSH
    0 EQINT
    <{
      48 PUSHINT
      NEWC
      8 STU
      ENDC
      CTOS
      ?fun_72 CALLDICT
    }> PUSHCONT
    IF
    <{
      s1 PUSH
      0 GTINT
    }> PUSHCONT
    <{
      s1 PUSH
      10 PUSHINT
      MOD
      48 ADDCONST
      NEWC
      8 STU
      ENDC
      CTOS
      s0 s1 XCHG
      ?fun_73 CALLDICT
      s0 s1 XCHG
      10 PUSHINT
      DIV
      s0 s1 XCHG
    }> PUSHCONT
    WHILE
    s1 POP
    ?fun_73 CALLDICT
  }>
  ?fun_75 PROC:<{
    ?fun_74 CALLDICT
  }>
  ?fun_76 PROC:<{
    x{} PUSHSLICE
    s1 PUSH
    0 EQINT
    <{
      48 PUSHINT
      NEWC
      8 STU
      ENDC
      CTOS
      ?fun_72 CALLDICT
    }> PUSHCONT
    IF
    <{
      s1 PUSH
      0 GTINT
    }> PUSHCONT
    <{
      s1 PUSH
      4 MODPOW2
      10 LESSINT
      <{
        s1 PUSH
        4 MODPOW2
        48 ADDCONST
        NEWC
        8 STU
        ENDC
        CTOS
        s0 s1 XCHG
        ?fun_73 CALLDICT
      }> PUSHCONT
      <{
        s1 PUSH
        4 MODPOW2
        65 ADDCONST
        -10 ADDCONST
        NEWC
        8 STU
        ENDC
        CTOS
        s0 s1 XCHG
        ?fun_73 CALLDICT
      }> PUSHCONT
      IFELSE
      s0 s1 XCHG
      4 RSHIFT
      s0 s1 XCHG
    }> PUSHCONT
    WHILE
    s1 POP
    ?fun_73 CALLDICT
  }>
  ?fun_78 PROC:<{
    s0 s1 XCHG
    x{3A20} PUSHSLICE
    ?fun_73 CALLDICT
    s0 s1 XCHG
    ?fun_75 CALLDICT
    1 4 DEBUG
    s0 POP
  }>
  ?fun_88 PROC:<{
    s0 PUSH
    3 EQINT
    <{
      s0 POP
      x{75696E7438} PUSHSLICE
    }> PUSHCONT
    IFJMP
    s0 PUSH
    4 EQINT
    <{
      s0 POP
      x{75696E743136} PUSHSLICE
    }> PUSHCONT
    IFJMP
    s0 PUSH
    5 EQINT
    <{
      s0 POP
      x{75696E743332} PUSHSLICE
    }> PUSHCONT
    IFJMP
    s0 PUSH
    6 EQINT
    <{
      s0 POP
      x{75696E743634} PUSHSLICE
    }> PUSHCONT
    IFJMP
    s0 PUSH
    8 EQINT
    <{
      s0 POP
      x{75696E74323536} PUSHSLICE
    }> PUSHCONT
    IFJMP
    s0 PUSH
    7 EQINT
    <{
      s0 POP
      x{636F696E73} PUSHSLICE
    }> PUSHCONT
    IFJMP
    s0 PUSH
    8 EQINT
    <{
      s0 POP
      x{61646472657373} PUSHSLICE
    }> PUSHCONT
    IFJMP
    s0 PUSH
    9 EQINT
    <{
      s0 POP
      x{64696374323536} PUSHSLICE
    }> PUSHCONT
    IFJMP
    s0 PUSH
    9 EQINT
    <{
      s0 POP
      x{6F626A526566} PUSHSLICE
    }> IFJMPREF
    s0 PUSH
    9 EQINT
    <{
      s0 POP
      x{63656C6C526566} PUSHSLICE
    }> IFJMPREF
    0 EQINT
    <{
      x{626F6F6C} PUSHSLICE
    }> PUSHCONT
    IFJMP
    x{756E6B6E6F776E} PUSHSLICE
  }>
  ?fun_90 PROC:<{
    c2 SAVE
    SAMEALTSAVE
    DUP2
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    4 PUSHINT
    SDSUBSTR
    4 PLDU
    s0 PUSH
    s0 PUSH
    9 LESSINT
    <{
      POW2
    }> PUSHCONT
    <{
      s0 POP
      0 PUSHINT
    }> PUSHCONT
    IFELSE
    s1 PUSH
    0 EQINT
    <{
      DROP2
      1 PUSHINT
      s0 s2 XCHG
      CTOS
      s0 s1 XCHG
      18 MULCONST
      80 ADDCONST
      DUP2
      4 ADDCONST
      2 PUSHINT
      SDSUBSTR
      2 PLDU
      s2 s3 PUXC
      6 ADDCONST
      10 PUSHINT
      SDSUBSTR
      10 PLDU
      s1 PUSH
      0 EQINT
      <{
        s1 POP
        s2 PUSH
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      <{
        ROTREV
        PLDREFVAR
        CTOS
        s1 s2 XCPU
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      IFELSE
      0 NEQINT
    }> PUSHCONT
    IFJMP
    s0 s1 XCHG
    9 LESSINT
    <{
      s0 s2 XCHG
      CTOS
      s0 s1 XCHG
      18 MULCONST
      80 ADDCONST
      DUP2
      4 ADDCONST
      2 PUSHINT
      SDSUBSTR
      2 PLDU
      s2 s3 PUXC
      6 ADDCONST
      10 PUSHINT
      SDSUBSTR
      10 PLDU
      s1 PUSH
      0 EQINT
      <{
        s1 POP
        s2 PUSH
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      <{
        ROTREV
        PLDREFVAR
        CTOS
        s1 s2 XCPU
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      IFELSE
      RETALT
    }> IFJMPREF
    s0 POP
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      PLDREFVAR
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s0 s1 XCHG
      PLDREFVAR
    }> PUSHCONT
    IFELSE
  }>
  ?fun_95 PROC:<{
    s1 PUSH
    HASHCU
    68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
    EQUAL
    <{
      s0 POP
    }> PUSHCONT
    IFJMP
    s0 s1 XCHG
    8 PUSHPOW2
    DICTUDEL
    s0 POP
    s0 PUSH
    <{
      <{
        CTOS
        s0 POP
        -1 PUSHINT
      }> PUSHCONT
      <{
        DROP2
        0 PUSHINT
      }> PUSHCONT
      TRY
    }> PUSHCONT
    1 1 CALLXARGS
    <{
      s0 POP
      x{} PUSHREF
    }> PUSHCONT
    IFNOT
  }>
  ?fun_112 PROC:<{
    s0 PUSH
    1 INDEX
    0 PUSHINT
    s2 PUSH
    2 INDEX
    s0 s3 XCHG
    3 INDEX
    s1 s3 s0 XCHG3
    s0 s1 XCHG
    1 GETGLOB
    4 INDEX
    s0 s1 XCHG
    NEWC
    32 STU
    64 STU
    ROT
    STGRAMS
    1 GETGLOB
    9 INDEX
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
    STSLICER
    STREF
    ENDC
    NULL
    0 PUSHINT
    24 PUSHINT
    NEWC
    6 STU
    s0 s4 XCHG
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
    s1 s4 XCHG
    STSLICER
    0 PUSHINT
    STGRAMS
    s1 PUSH
    ISNULL
    <{
      1 PUSHINT
      s2 POP
      107 STU
    }> PUSHCONT
    <{
      7 PUSHINT
      s0 s1 XCHG
      108 STU
      STREF
    }> PUSHCONT
    IFELSE
    STREF
    ENDC
    7 PUSHPOW2
    ROT
    OR
    SENDRAWMSG
    0 PUSHINT
  }>
  ?fun_127 PROC:<{
    16 PUSHINT
    0 PUSHINT
    MYADDR
    11 PUSHINT
    8 PUSHPOW2
    SDSUBSTR
    256 PLDU
    s1 PUSH
    3812333683 PUSHINT
    s0 s5 XCHG
    1 INDEX
    s1 s5 s0 XCHG3
    s0 s1 XCHG
    1 GETGLOB
    4 INDEX
    s0 s1 XCHG
    NEWC
    32 STU
    64 STU
    ROT
    STGRAMS
    1 GETGLOB
    9 INDEX
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
    STSLICER
    STREF
    ENDC
    s1 s3 s0 XCHG3
    1 PUSHINT
    1 PUSHINT
    s0 s5 XCHG
    NEWC
    6 STU
    s0 s3 XCHG
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
    s1 s3 XCHG
    STSLICER
    s0 s3 XCHG2
    STGRAMS
    s1 s3 XCHG
    107 STU
    STREF
    ENDC
    s0 s1 XCHG
    SENDRAWMSG
    -1 PUSHINT
  }>
  ?fun_140 PROC:<{
    c4 PUSHCTR
    0 PUSHINT
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      PLDREFVAR
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s0 s1 XCHG
      PLDREFVAR
    }> PUSHCONT
    IFELSE
    1 PUSHINT
    1 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    0 NEQINT
    1690 THROWIFNOT
  }>
  ?fun_141 PROC:<{
    1 GETGLOB
    1 INDEX
    c4 PUSHCTR
    7 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    EQUAL
    550 THROWIFNOT
  }>
  ?fun_142 PROC:<{
  }>
  ?fun_143 PROC:<{
    125767 PUSHINT
    c3 PUSHCTR
    EXECUTE
    0 PUSHINT
    67678 PUSHINT
    c3 PUSHCTR
    EXECUTE
    ?fun_45 CALLDICT
    s0 s1 XCHG
    0 PUSHINT
    93365 PUSHINT
    c3 PUSHCTR
    EXECUTE
    EQUAL
    492 THROWIFNOT
    c4 PUSHCTR
    0 PUSHINT
    DUP2
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      PLDREFVAR
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s0 s1 XCHG
      PLDREFVAR
    }> PUSHCONT
    IFELSE
    1 PUSHINT
    -1 PUSHINT
    ?fun_ref_ec59f05d6c9ac356 CALLREF
    ?fun_ref_ec59f05d6c9ac356 CALLREF
    c4 POPCTR
  }>
  ?fun_144 PROC:<{
  }>
  ?fun_145 PROC:<{
  }>
  ?fun_146 PROC:<{
  }>
  ?fun_147 PROC:<{
    c4 PUSHCTR
    0 PUSHINT
    1 TUPLE
    s1 POP
    s0 s1 XCHG
    CTOS
    s0 PUSH
    0 PUSHINT
    PLDREFVAR
    s0 s1 XCHG
    1 PUSHINT
    PLDREFVAR
    c4 POPCTR
    SETCODE
  }>
  ?fun_152 PROC:<{
  }>
  ?fun_153 PROC:<{
    c4 PUSHCTR
    0 PUSHINT
    1 TUPLE
    4 PUSHINT
    s3 PUSH
    CTOS
    350 PUSHINT
    8 PUSHPOW2
    SDSUBSTR
    256 PLDU
    s2 s3 XCHG_IJ
    ?fun_ref_ec59f05d6c9ac356 CALLREF
    c4 POPCTR
    6040104136285857541260900362685093296432500 PUSHINT
    ROT
    x{} PUSHREF
    ROTREV
    435778055796 PUSHINT
    s0 s3 XCHG
    7850279558805522911016931325 PUSHINT
    NEWC
    134 STU
    216 PUSHINT
    STONES
    s1 s3 XCHG
    256 STU
    STREF
    STREF
    ENDC
    2 TUPLE
    TPUSH
  }>
  ?fun_154 PROC:<{
    c4 PUSHCTR
    0 PUSHINT
    1 TUPLE
    1 PUSHINT
    s3 PUSH
    CTOS
    350 PUSHINT
    8 PUSHPOW2
    SDSUBSTR
    256 PLDU
    s2 s3 XCHG_IJ
    ?fun_ref_ec59f05d6c9ac356 CALLREF
    c4 POPCTR
    7302037843517092141845206072946930925372494662891926125872950764916 PUSHINT
    ROT
    x{} PUSHREF
    ROTREV
    435778055796 PUSHINT
    s0 s3 XCHG
    7850279558805522911016931325 PUSHINT
    NEWC
    134 STU
    216 PUSHINT
    STONES
    s1 s3 XCHG
    256 STU
    STREF
    STREF
    ENDC
    2 TUPLE
    TPUSH
  }>
  ?fun_155 PROC:<{
    c4 PUSHCTR
    0 PUSHINT
    1 TUPLE
    2 PUSHINT
    s3 PUSH
    CTOS
    350 PUSHINT
    8 PUSHPOW2
    SDSUBSTR
    256 PLDU
    s2 s3 XCHG_IJ
    ?fun_ref_ec59f05d6c9ac356 CALLREF
    c4 POPCTR
    7302037843517092168089036360352778575740500089818215937372207670644 PUSHINT
    ROT
    x{} PUSHREF
    ROTREV
    435778055796 PUSHINT
    s0 s3 XCHG
    7850279558805522911016931325 PUSHINT
    NEWC
    134 STU
    216 PUSHINT
    STONES
    s1 s3 XCHG
    256 STU
    STREF
    STREF
    ENDC
    2 TUPLE
    TPUSH
  }>
  ?fun_156 PROC:<{
  }>
  ?fun_157 PROC:<{
  }>
  ?fun_66977 PROC:<{
    s0 s1 XCHG
    130966 PUSHINT
    c3 PUSHCTR
    EXECUTE
    s0 s1 XCHG
    s1 PUSH
    HASHCU
    68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
    EQUAL
    <{
      DROP2
      NULL
      0 PUSHINT
    }> PUSHCONT
    <{
      s0 s1 XCHG
      8 PUSHPOW2
      DICTUGET
      NULLSWAPIFNOT
    }> PUSHCONT
    IFELSE
    <{
      256 PLDU
      -1 PUSHINT
    }> PUSHCONT
    <{
      s0 POP
      0 PUSHINT
      0 PUSHINT
    }> PUSHCONT
    IFELSE
    IFRET
    s0 POP
    -1 PUSHINT
  }>
  ?fun_67678 PROC:<{
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      PLDREFVAR
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s0 s1 XCHG
      PLDREFVAR
    }> PUSHCONT
    IFELSE
  }>
  ?fun_68118 PROC:<{
    s1 PUSH
    HASHCU
    68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
    EQUAL
    <{
      DROP2
      NULL
      0 PUSHINT
    }> PUSHCONT
    <{
      s0 s1 XCHG
      8 PUSHPOW2
      DICTUGET
      NULLSWAPIFNOT
    }> PUSHCONT
    IFELSE
    <{
      256 PLDU
      -1 PUSHINT
    }> PUSHCONT
    <{
      s0 POP
      0 PUSHINT
      0 PUSHINT
    }> PUSHCONT
    IFELSE
  }>
  ?fun_68315 PROC:<{
    18406256064951155 PUSHINT
    8 PUSHINT
    ROT
    2 TUPLE
    1 TUPLE
    NULL
    NEWC
    2 TUPLE
    s1 PUSH
    TLEN
    s0 s3 XCHG
    NEWC
    80 STU
    1 PUSHINT
    s0 PUSH
    2 PUSHINT
    350 PUSHINT
    0 PUSHINT
    s0 PUSH
    <{
      s0 s9 PUSH2
      LESS
    }> PUSHCONT
    <{
      s8 s0 PUSH2
      INDEXVAR
      s0 PUSH
      0 INDEX
      s0 PUSH
      s0 PUSH
      9 LESSINT
      <{
        POW2
      }> PUSHCONT
      <{
        s0 POP
        0 PUSHINT
      }> PUSHCONT
      IFELSE
      s0 PUSH
      0 GTINT
      <{
        s5 s0 PUSH2
        ADD
        10 PUSHPOW2DEC
        GREATER
        <{
          s5 POP
          s0 s7 XCHG
          INC
          0 PUSHINT
          s10 PUSH
          TLEN
          s2 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s8 s5 XCHG2
        }> PUSHCONT
        IF
      }> PUSHCONT
      <{
        s4 PUSH
        INC
        s7 PUSH
        GREATER
        <{
          s4 POP
          s5 POP
          s0 s5 XCHG
          INC
          0 PUSHINT
          4 PUSHINT
          s10 PUSH
          TLEN
          s3 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s7 s6 s6 XCHG3
          s0 s4 XCHG
        }> PUSHCONT
        IF
      }> PUSHCONT
      IFELSE
      s1 PUSH
      9 LESSINT
      <{
        s1 PUSH
        9 EQINT
        <{
          s0 s2 XCHG
          1 INDEX
          s10 s7 PUSH2
          INDEXVAR
          STREF
          s10 s0 s7 XC2PU
          SETINDEXVAR
        }> PUSHCONT
        <{
          s2 POP
          1059 THROW
          s0 s9 XCHG
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      <{
        s0 s2 XCHG
        1 INDEX
        ABS
        s10 s8 PUSH2
        INDEXVAR
        s3 PUSH
        STUX
        s10 s0 s8 XC2PU
        SETINDEXVAR
      }> IFREFELSE
      s9 s8 XCHG2
      4 STU
      s8 PUSH
      0 GTINT
      <{
        3 PUSHINT
        s7 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s7 PUSH
        }> PUSHCONT
        IFELSE
        ROT
        2 STU
        s4 s1 PUXC
        10 STU
        2 STU
        s3 s8 XCHG2
        ADD
      }> <{
        s8 POP
        10 PUSHPOW2DEC
        s5 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s5 PUSH
        }> PUSHCONT
        IFELSE
        s0 s9 XCHG2
        2 STU
        s1 s8 XCHG
        10 STU
        s1 s1 PUXC
        2 STU
        s0 s1 XCHG
        INC
        s7 s0 s7 XCHG3
      }> IFREFELSEREF
      s0 s2 XCHG
      INC
      s6 s7 XCHG_IJ
    }> PUSHREFCONT
    WHILE
    6 BLKDROP
    2 2 BLKDROP2
    s1 PUSH
    1 INDEX
    s2 PUSH
    TLEN
    DEC
    s0 PUSH
    1 GTINT
    <{
      s1 PUSH
      BREFS
      0 EQINT
      <{
        x{} PUSHREF
        x{} PUSHREF
        s0 s3 XCHG2
        STREF
        s1 s2 XCHG
        STREF
      }> PUSHCONT
      <{
        s1 PUSH
        BREFS
        1 EQINT
        <{
          x{} PUSHREF
          ROT
          STREF
          s0 s1 XCHG
        }> PUSHCONT
        IF
        s0 s1 XCHG
      }> PUSHCONT
      IFELSE
      s0 s1 XCHG
    }> IFREF
    350 PUSHINT
    s3 PUSH
    BBITS
    SUB
    s1 s3 XCHG
    STONES
    s0 s1 XCHG
    STBR
    s1 PUSH
    1 EQINT
    <{
      2 1 BLKDROP2
      ENDC
    }> PUSHCONT
    <{
      s0 s1 XCHG
      2 EQINT
      <{
        s0 s1 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        ENDC
      }> PUSHCONT
      IFJMP
      s1 PUSH
      3 INDEX
      ENDC
      s0 s2 XCHG
      2 INDEX
      ENDC
      s0 s1 XCHG
      STREF
      STREF
      ENDC
    }> PUSHCONT
    IFELSE
  }>
  ?fun_68648 PROC:<{
    c4 PUSHCTR
    0 PUSHINT
    1 TUPLE
    1 GETGLOB
    10 INDEX
    0 PUSHINT
    1 GETGLOB
    s0 s1 XCHG
    10 PUSHINT
    SETINDEXVAR
    1 SETGLOB
    s0 s3 XCHG
    CTOS
    350 PUSHINT
    8 PUSHPOW2
    SDSUBSTR
    256 PLDU
    s1 s3 BLKSWAP
    s0 s3 XCHG
    1 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    s0 s1 s3 XCHG3
    s1 PUSH
    544943221246095313366894 PUSHINT
    s4 s1 BLKSWAP
    5 TUPLE
    TPUSH
  }>
  ?fun_68968 PROC:<{
  }>
  ?fun_71979 PROC:<{
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
  }>
  ?fun_72147 PROC:<{
    s0 PUSH
    HASHCU
    68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
    EQUAL
    <{
      s0 POP
      1314212940 PUSHINT
    }> PUSHCONT
    <{
      CTOS
      80 PLDU
    }> PUSHCONT
    IFELSE
  }>
  ?fun_72361 PROC:<{
    16 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_72737 PROC:<{
    ROTREV
    93729 PUSHINT
    c3 PUSHCTR
    EXECUTE
    s0 s1 XCHG
    s1 PUSH
    HASHCU
    68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
    EQUAL
    <{
      DROP2
      NULL
      0 PUSHINT
    }> PUSHCONT
    <{
      s0 s1 XCHG
      8 PUSHPOW2
      DICTUGETREF
      NULLSWAPIFNOT
      <{
        s0 POP
        NULL
        0 PUSHINT
      }> PUSHCONT
      IFNOTJMP
      -1 PUSHINT
    }> PUSHCONT
    IFELSE
    IFRET
    s0 POP
    x{} PUSHREF
  }>
  ?fun_73824 PROC:<{
    10 GETPARAM
    c4 PUSHCTR
    6 PUSHINT
    NEWC
    5 STU
    s1 s2 XCHG
    STREF
    STREF
    ENDC
    16 PUSHPOW2DEC
    CDATASIZE
    8 THROWIFNOT
    BALANCE
    0 INDEX
    s0 s3 XCHG
    0 PUSHINT
    GETSTORAGEFEE
    SUB
    0 PUSHINT
    s0 s1 XCHG
    MAX
  }>
  ?fun_77514 PROC:<{
    371002272884 PUSHINT
    9 PUSHINT
    s0 s2 XCHG
    1814802986876646945381 PUSHINT
    8 PUSHINT
    ROT
    2 TUPLE
    0 PUSHINT
    0 PUSHINT
    2 TUPLE
    0 PUSHINT
    0 PUSHINT
    2 TUPLE
    9 PUSHINT
    x{} PUSHREF
    2 TUPLE
    4 TUPLE
    NULL
    NEWC
    2 TUPLE
    s1 PUSH
    TLEN
    s0 s3 XCHG
    NEWC
    80 STU
    1 PUSHINT
    s0 PUSH
    2 PUSHINT
    350 PUSHINT
    0 PUSHINT
    s0 PUSH
    <{
      s0 s9 PUSH2
      LESS
    }> PUSHCONT
    <{
      s8 s0 PUSH2
      INDEXVAR
      s0 PUSH
      0 INDEX
      s0 PUSH
      s0 PUSH
      9 LESSINT
      <{
        POW2
      }> PUSHCONT
      <{
        s0 POP
        0 PUSHINT
      }> PUSHCONT
      IFELSE
      s0 PUSH
      0 GTINT
      <{
        s5 s0 PUSH2
        ADD
        10 PUSHPOW2DEC
        GREATER
        <{
          s5 POP
          s0 s7 XCHG
          INC
          0 PUSHINT
          s10 PUSH
          TLEN
          s2 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s8 s5 XCHG2
        }> PUSHCONT
        IF
      }> PUSHCONT
      <{
        s4 PUSH
        INC
        s7 PUSH
        GREATER
        <{
          s4 POP
          s5 POP
          s0 s5 XCHG
          INC
          0 PUSHINT
          4 PUSHINT
          s10 PUSH
          TLEN
          s3 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s7 s6 s6 XCHG3
          s0 s4 XCHG
        }> PUSHCONT
        IF
      }> PUSHCONT
      IFELSE
      s1 PUSH
      9 LESSINT
      <{
        s1 PUSH
        9 EQINT
        <{
          s0 s2 XCHG
          1 INDEX
          s10 s7 PUSH2
          INDEXVAR
          STREF
          s10 s0 s7 XC2PU
          SETINDEXVAR
        }> PUSHCONT
        <{
          s2 POP
          1059 THROW
          s0 s9 XCHG
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      <{
        s0 s2 XCHG
        1 INDEX
        ABS
        s10 s8 PUSH2
        INDEXVAR
        s3 PUSH
        STUX
        s10 s0 s8 XC2PU
        SETINDEXVAR
      }> IFREFELSE
      s9 s8 XCHG2
      4 STU
      s8 PUSH
      0 GTINT
      <{
        3 PUSHINT
        s7 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s7 PUSH
        }> PUSHCONT
        IFELSE
        ROT
        2 STU
        s4 s1 PUXC
        10 STU
        2 STU
        s3 s8 XCHG2
        ADD
      }> <{
        s8 POP
        10 PUSHPOW2DEC
        s5 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s5 PUSH
        }> PUSHCONT
        IFELSE
        s0 s9 XCHG2
        2 STU
        s1 s8 XCHG
        10 STU
        s1 s1 PUXC
        2 STU
        s0 s1 XCHG
        INC
        s7 s0 s7 XCHG3
      }> IFREFELSEREF
      s0 s2 XCHG
      INC
      s6 s7 XCHG_IJ
    }> PUSHREFCONT
    WHILE
    6 BLKDROP
    2 2 BLKDROP2
    s1 PUSH
    1 INDEX
    s2 PUSH
    TLEN
    DEC
    s0 PUSH
    1 GTINT
    <{
      s1 PUSH
      BREFS
      0 EQINT
      <{
        x{} PUSHREF
        x{} PUSHREF
        s0 s3 XCHG2
        STREF
        s1 s2 XCHG
        STREF
      }> PUSHCONT
      <{
        s1 PUSH
        BREFS
        1 EQINT
        <{
          x{} PUSHREF
          ROT
          STREF
          s0 s1 XCHG
        }> PUSHCONT
        IF
        s0 s1 XCHG
      }> PUSHCONT
      IFELSE
      s0 s1 XCHG
    }> IFREF
    350 PUSHINT
    s3 PUSH
    BBITS
    SUB
    s1 s3 XCHG
    STONES
    s0 s1 XCHG
    STBR
    s1 PUSH
    1 EQINT
    <{
      2 1 BLKDROP2
      ENDC
    }> PUSHCONT
    <{
      s0 s1 XCHG
      2 EQINT
      <{
        s0 s1 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        ENDC
      }> PUSHCONT
      IFJMP
      s1 PUSH
      3 INDEX
      ENDC
      s0 s2 XCHG
      2 INDEX
      ENDC
      s0 s1 XCHG
      STREF
      STREF
      ENDC
    }> PUSHCONT
    IFELSE
    s1 s2 XCHG
    2 TUPLE
    8 PUSHINT
    0 PUSHINT
    2 TUPLE
    8 PUSHINT
    0 PUSHINT
    2 TUPLE
    7 PUSHINT
    0 PUSHINT
    2 TUPLE
    8 PUSHINT
    0 PUSHINT
    2 TUPLE
    8 PUSHINT
    0 PUSHINT
    2 TUPLE
    8 PUSHINT
    0 PUSHINT
    2 TUPLE
    8 PUSHINT
    0 PUSHINT
    2 TUPLE
    6 PUSHINT
    0 PUSHINT
    2 TUPLE
    6 PUSHINT
    0 PUSHINT
    2 TUPLE
    7 PUSHINT
    0 PUSHINT
    2 TUPLE
    6 PUSHINT
    0 PUSHINT
    2 TUPLE
    12 TUPLE
    NULL
    NEWC
    2 TUPLE
    s1 PUSH
    TLEN
    s0 s3 XCHG
    NEWC
    80 STU
    1 PUSHINT
    s0 PUSH
    2 PUSHINT
    350 PUSHINT
    0 PUSHINT
    s0 PUSH
    <{
      s0 s9 PUSH2
      LESS
    }> PUSHCONT
    <{
      s8 s0 PUSH2
      INDEXVAR
      s0 PUSH
      0 INDEX
      s0 PUSH
      s0 PUSH
      9 LESSINT
      <{
        POW2
      }> PUSHCONT
      <{
        s0 POP
        0 PUSHINT
      }> PUSHCONT
      IFELSE
      s0 PUSH
      0 GTINT
      <{
        s5 s0 PUSH2
        ADD
        10 PUSHPOW2DEC
        GREATER
        <{
          s5 POP
          s0 s7 XCHG
          INC
          0 PUSHINT
          s10 PUSH
          TLEN
          s2 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s8 s5 XCHG2
        }> PUSHCONT
        IF
      }> PUSHCONT
      <{
        s4 PUSH
        INC
        s7 PUSH
        GREATER
        <{
          s4 POP
          s5 POP
          s0 s5 XCHG
          INC
          0 PUSHINT
          4 PUSHINT
          s10 PUSH
          TLEN
          s3 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s7 s6 s6 XCHG3
          s0 s4 XCHG
        }> PUSHCONT
        IF
      }> PUSHCONT
      IFELSE
      s1 PUSH
      9 LESSINT
      <{
        s1 PUSH
        9 EQINT
        <{
          s0 s2 XCHG
          1 INDEX
          s10 s7 PUSH2
          INDEXVAR
          STREF
          s10 s0 s7 XC2PU
          SETINDEXVAR
        }> PUSHCONT
        <{
          s2 POP
          1059 THROW
          s0 s9 XCHG
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      <{
        s0 s2 XCHG
        1 INDEX
        ABS
        s10 s8 PUSH2
        INDEXVAR
        s3 PUSH
        STUX
        s10 s0 s8 XC2PU
        SETINDEXVAR
      }> IFREFELSE
      s9 s8 XCHG2
      4 STU
      s8 PUSH
      0 GTINT
      <{
        3 PUSHINT
        s7 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s7 PUSH
        }> PUSHCONT
        IFELSE
        ROT
        2 STU
        s4 s1 PUXC
        10 STU
        2 STU
        s3 s8 XCHG2
        ADD
      }> <{
        s8 POP
        10 PUSHPOW2DEC
        s5 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s5 PUSH
        }> PUSHCONT
        IFELSE
        s0 s9 XCHG2
        2 STU
        s1 s8 XCHG
        10 STU
        s1 s1 PUXC
        2 STU
        s0 s1 XCHG
        INC
        s7 s0 s7 XCHG3
      }> IFREFELSEREF
      s0 s2 XCHG
      INC
      s6 s7 XCHG_IJ
    }> PUSHREFCONT
    WHILE
    6 BLKDROP
    2 2 BLKDROP2
    s1 PUSH
    1 INDEX
    s2 PUSH
    TLEN
    DEC
    s0 PUSH
    1 GTINT
    <{
      s1 PUSH
      BREFS
      0 EQINT
      <{
        x{} PUSHREF
        x{} PUSHREF
        s0 s3 XCHG2
        STREF
        s1 s2 XCHG
        STREF
      }> PUSHCONT
      <{
        s1 PUSH
        BREFS
        1 EQINT
        <{
          x{} PUSHREF
          ROT
          STREF
          s0 s1 XCHG
        }> PUSHCONT
        IF
        s0 s1 XCHG
      }> PUSHCONT
      IFELSE
      s0 s1 XCHG
    }> IFREF
    350 PUSHINT
    s3 PUSH
    BBITS
    SUB
    s1 s3 XCHG
    STONES
    s0 s1 XCHG
    STBR
    s1 PUSH
    1 EQINT
    <{
      2 1 BLKDROP2
      ENDC
    }> PUSHCONT
    <{
      s0 s1 XCHG
      2 EQINT
      <{
        s0 s1 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        ENDC
      }> PUSHCONT
      IFJMP
      s1 PUSH
      3 INDEX
      ENDC
      s0 s2 XCHG
      2 INDEX
      ENDC
      s0 s1 XCHG
      STREF
      STREF
      ENDC
    }> PUSHCONT
    IFELSE
  }>
  ?fun_77798 PROC:<{
  }>
  ?fun_78847 PROC:<{
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_78898 PROC:<{
    6 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_79569 PROC:<{
    435778055796 PUSHINT
    8 PUSHINT
    s0 s4 XCHG2
    2 TUPLE
    9 PUSHINT
    s0 s3 XCHG2
    2 TUPLE
    9 PUSHINT
    ROT
    2 TUPLE
    3 TUPLE
    NULL
    NEWC
    2 TUPLE
    s1 PUSH
    TLEN
    s0 s3 XCHG
    NEWC
    80 STU
    1 PUSHINT
    s0 PUSH
    2 PUSHINT
    350 PUSHINT
    0 PUSHINT
    s0 PUSH
    <{
      s0 s9 PUSH2
      LESS
    }> PUSHCONT
    <{
      s8 s0 PUSH2
      INDEXVAR
      s0 PUSH
      0 INDEX
      s0 PUSH
      s0 PUSH
      9 LESSINT
      <{
        POW2
      }> PUSHCONT
      <{
        s0 POP
        0 PUSHINT
      }> PUSHCONT
      IFELSE
      s0 PUSH
      0 GTINT
      <{
        s5 s0 PUSH2
        ADD
        10 PUSHPOW2DEC
        GREATER
        <{
          s5 POP
          s0 s7 XCHG
          INC
          0 PUSHINT
          s10 PUSH
          TLEN
          s2 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s8 s5 XCHG2
        }> PUSHCONT
        IF
      }> PUSHCONT
      <{
        s4 PUSH
        INC
        s7 PUSH
        GREATER
        <{
          s4 POP
          s5 POP
          s0 s5 XCHG
          INC
          0 PUSHINT
          4 PUSHINT
          s10 PUSH
          TLEN
          s3 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s7 s6 s6 XCHG3
          s0 s4 XCHG
        }> PUSHCONT
        IF
      }> PUSHCONT
      IFELSE
      s1 PUSH
      9 LESSINT
      <{
        s1 PUSH
        9 EQINT
        <{
          s0 s2 XCHG
          1 INDEX
          s10 s7 PUSH2
          INDEXVAR
          STREF
          s10 s0 s7 XC2PU
          SETINDEXVAR
        }> PUSHCONT
        <{
          s2 POP
          1059 THROW
          s0 s9 XCHG
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      <{
        s0 s2 XCHG
        1 INDEX
        ABS
        s10 s8 PUSH2
        INDEXVAR
        s3 PUSH
        STUX
        s10 s0 s8 XC2PU
        SETINDEXVAR
      }> IFREFELSE
      s9 s8 XCHG2
      4 STU
      s8 PUSH
      0 GTINT
      <{
        3 PUSHINT
        s7 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s7 PUSH
        }> PUSHCONT
        IFELSE
        ROT
        2 STU
        s4 s1 PUXC
        10 STU
        2 STU
        s3 s8 XCHG2
        ADD
      }> <{
        s8 POP
        10 PUSHPOW2DEC
        s5 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s5 PUSH
        }> PUSHCONT
        IFELSE
        s0 s9 XCHG2
        2 STU
        s1 s8 XCHG
        10 STU
        s1 s1 PUXC
        2 STU
        s0 s1 XCHG
        INC
        s7 s0 s7 XCHG3
      }> IFREFELSEREF
      s0 s2 XCHG
      INC
      s6 s7 XCHG_IJ
    }> PUSHREFCONT
    WHILE
    6 BLKDROP
    2 2 BLKDROP2
    s1 PUSH
    1 INDEX
    s2 PUSH
    TLEN
    DEC
    s0 PUSH
    1 GTINT
    <{
      s1 PUSH
      BREFS
      0 EQINT
      <{
        x{} PUSHREF
        x{} PUSHREF
        s0 s3 XCHG2
        STREF
        s1 s2 XCHG
        STREF
      }> PUSHCONT
      <{
        s1 PUSH
        BREFS
        1 EQINT
        <{
          x{} PUSHREF
          ROT
          STREF
          s0 s1 XCHG
        }> PUSHCONT
        IF
        s0 s1 XCHG
      }> PUSHCONT
      IFELSE
      s0 s1 XCHG
    }> IFREF
    350 PUSHINT
    s3 PUSH
    BBITS
    SUB
    s1 s3 XCHG
    STONES
    s0 s1 XCHG
    STBR
    s1 PUSH
    1 EQINT
    <{
      2 1 BLKDROP2
      ENDC
    }> PUSHCONT
    <{
      s0 s1 XCHG
      2 EQINT
      <{
        s0 s1 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        ENDC
      }> PUSHCONT
      IFJMP
      s1 PUSH
      3 INDEX
      ENDC
      s0 s2 XCHG
      2 INDEX
      ENDC
      s0 s1 XCHG
      STREF
      STREF
      ENDC
    }> PUSHCONT
    IFELSE
  }>
  ?fun_80971 PROC:<{
  }>
  ?fun_81463 PROC:<{
    4 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_81750 PROC:<{
    4825089589890555708795 PUSHINT
    NEWC
    98 STU
    252 PUSHINT
    STONES
    256 STU
    ENDC
  }>
  ?fun_82580 PROC:<{
    10 PUSHINT
    7 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_82987 PROC:<{
    9 PUSHINT
    64 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_83291 PROC:<{
    s2 PUSH
    HASHCU
    68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
    EQUAL
    <{
      1 2 BLKDROP2
      NULL
      s1 s2 XCHG
      8 PUSHPOW2
      DICTUSETREF
    }> PUSHCONT
    <{
      s0 s2 XCHG
      8 PUSHPOW2
      DICTUSETREF
    }> PUSHCONT
    IFELSE
  }>
  ?fun_85239 PROC:<{
    s0 s1 XCHG
    130966 PUSHINT
    c3 PUSHCTR
    EXECUTE
    s0 s1 PUSH2
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    4 PUSHINT
    SDSUBSTR
    4 PLDU
    s0 PUSH
    9 LESSINT
    <{
      POW2
    }> PUSHCONT
    <{
      s0 POP
      0 PUSHINT
    }> PUSHCONT
    IFELSE
    s1 s2 XCHG
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_87801 PROC:<{
  }>
  ?fun_88345 PROC:<{
    332395405930 PUSHINT
    9 PUSHINT
    s0 s3 XCHG2
    2 TUPLE
    9 PUSHINT
    ROT
    2 TUPLE
    2 TUPLE
    NULL
    NEWC
    2 TUPLE
    s1 PUSH
    TLEN
    s0 s3 XCHG
    NEWC
    80 STU
    1 PUSHINT
    s0 PUSH
    2 PUSHINT
    350 PUSHINT
    0 PUSHINT
    s0 PUSH
    <{
      s0 s9 PUSH2
      LESS
    }> PUSHCONT
    <{
      s8 s0 PUSH2
      INDEXVAR
      s0 PUSH
      0 INDEX
      s0 PUSH
      s0 PUSH
      9 LESSINT
      <{
        POW2
      }> PUSHCONT
      <{
        s0 POP
        0 PUSHINT
      }> PUSHCONT
      IFELSE
      s0 PUSH
      0 GTINT
      <{
        s5 s0 PUSH2
        ADD
        10 PUSHPOW2DEC
        GREATER
        <{
          s5 POP
          s0 s7 XCHG
          INC
          0 PUSHINT
          s10 PUSH
          TLEN
          s2 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s8 s5 XCHG2
        }> PUSHCONT
        IF
      }> PUSHCONT
      <{
        s4 PUSH
        INC
        s7 PUSH
        GREATER
        <{
          s4 POP
          s5 POP
          s0 s5 XCHG
          INC
          0 PUSHINT
          4 PUSHINT
          s10 PUSH
          TLEN
          s3 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s7 s6 s6 XCHG3
          s0 s4 XCHG
        }> PUSHCONT
        IF
      }> PUSHCONT
      IFELSE
      s1 PUSH
      9 LESSINT
      <{
        s1 PUSH
        9 EQINT
        <{
          s0 s2 XCHG
          1 INDEX
          s10 s7 PUSH2
          INDEXVAR
          STREF
          s10 s0 s7 XC2PU
          SETINDEXVAR
        }> PUSHCONT
        <{
          s2 POP
          1059 THROW
          s0 s9 XCHG
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      <{
        s0 s2 XCHG
        1 INDEX
        ABS
        s10 s8 PUSH2
        INDEXVAR
        s3 PUSH
        STUX
        s10 s0 s8 XC2PU
        SETINDEXVAR
      }> IFREFELSE
      s9 s8 XCHG2
      4 STU
      s8 PUSH
      0 GTINT
      <{
        3 PUSHINT
        s7 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s7 PUSH
        }> PUSHCONT
        IFELSE
        ROT
        2 STU
        s4 s1 PUXC
        10 STU
        2 STU
        s3 s8 XCHG2
        ADD
      }> <{
        s8 POP
        10 PUSHPOW2DEC
        s5 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s5 PUSH
        }> PUSHCONT
        IFELSE
        s0 s9 XCHG2
        2 STU
        s1 s8 XCHG
        10 STU
        s1 s1 PUXC
        2 STU
        s0 s1 XCHG
        INC
        s7 s0 s7 XCHG3
      }> IFREFELSEREF
      s0 s2 XCHG
      INC
      s6 s7 XCHG_IJ
    }> PUSHREFCONT
    WHILE
    6 BLKDROP
    2 2 BLKDROP2
    s1 PUSH
    1 INDEX
    s2 PUSH
    TLEN
    DEC
    s0 PUSH
    1 GTINT
    <{
      s1 PUSH
      BREFS
      0 EQINT
      <{
        x{} PUSHREF
        x{} PUSHREF
        s0 s3 XCHG2
        STREF
        s1 s2 XCHG
        STREF
      }> PUSHCONT
      <{
        s1 PUSH
        BREFS
        1 EQINT
        <{
          x{} PUSHREF
          ROT
          STREF
          s0 s1 XCHG
        }> PUSHCONT
        IF
        s0 s1 XCHG
      }> PUSHCONT
      IFELSE
      s0 s1 XCHG
    }> IFREF
    350 PUSHINT
    s3 PUSH
    BBITS
    SUB
    s1 s3 XCHG
    STONES
    s0 s1 XCHG
    STBR
    s1 PUSH
    1 EQINT
    <{
      2 1 BLKDROP2
      ENDC
    }> PUSHCONT
    <{
      s0 s1 XCHG
      2 EQINT
      <{
        s0 s1 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        ENDC
      }> PUSHCONT
      IFJMP
      s1 PUSH
      3 INDEX
      ENDC
      s0 s2 XCHG
      2 INDEX
      ENDC
      s0 s1 XCHG
      STREF
      STREF
      ENDC
    }> PUSHCONT
    IFELSE
  }>
  ?fun_88892 PROC:<{
    c4 PUSHCTR
    0 PUSHINT
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    PLDREFVAR
  }>
  ?fun_90518 PROC:<{
    s1 PUSH
    HASHCU
    68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
    EQUAL
    <{
      DROP2
      NULL
      0 PUSHINT
    }> PUSHCONT
    <{
      s0 s1 XCHG
      8 PUSHPOW2
      DICTUGETREF
      NULLSWAPIFNOT
      <{
        s0 POP
        NULL
        0 PUSHINT
      }> PUSHCONT
      IFNOTJMP
      -1 PUSHINT
    }> PUSHCONT
    IFELSE
  }>
  ?fun_92829 PROC:<{
    0 PUSHINT
    7 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_93365 PROC:<{
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_93729 PROC:<{
    s0 s1 XCHG
    130966 PUSHINT
    c3 PUSHCTR
    EXECUTE
    s0 s1 XCHG
    s1 PUSH
    HASHCU
    68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
    EQUAL
    <{
      DROP2
      NULL
      0 PUSHINT
    }> PUSHCONT
    <{
      s0 s1 XCHG
      8 PUSHPOW2
      DICTUGETREF
      NULLSWAPIFNOT
      <{
        s0 POP
        NULL
        0 PUSHINT
      }> PUSHCONT
      IFNOTJMP
      -1 PUSHINT
    }> PUSHCONT
    IFELSE
    IFRET
    s0 POP
    x{} PUSHREF
  }>
  ?fun_96161 PROC:<{
    ROTREV
    93729 PUSHINT
    c3 PUSHCTR
    EXECUTE
    s0 s1 XCHG
    s1 PUSH
    HASHCU
    68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
    EQUAL
    <{
      DROP2
      NULL
      0 PUSHINT
    }> PUSHCONT
    <{
      s0 s1 XCHG
      8 PUSHPOW2
      DICTUGET
      NULLSWAPIFNOT
    }> PUSHCONT
    IFELSE
    <{
      256 PLDU
      -1 PUSHINT
    }> PUSHCONT
    <{
      s0 POP
      0 PUSHINT
      0 PUSHINT
    }> PUSHCONT
    IFELSE
    IFRET
    s0 POP
    -1 PUSHINT
  }>
  ?fun_96546 PROC:<{
    11 PUSHINT
    64 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_96786 PROC:<{
    c4 PUSHCTR
    s0 s1 XCHG
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      PLDREFVAR
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s0 s1 XCHG
      PLDREFVAR
    }> PUSHCONT
    IFELSE
  }>
  ?fun_97013 PROC:<{
  }>
  ?fun_99093 PROC:<{
    s0 s1 XCHG
    101620 PUSHINT
    c3 PUSHCTR
    EXECUTE
    s0 s1 XCHG
    ?fun_90 CALLDICT
  }>
  ?fun_99339 PROC:<{
    8 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_100035 PROC:<{
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_100065 PROC:<{
    7 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_101620 PROC:<{
    c4 PUSHCTR
    s0 s1 XCHG
    ?fun_90 CALLDICT
  }>
  ?fun_102394 PROC:<{
    427070942835 PUSHINT
    7 PUSHINT
    ROT
    2 TUPLE
    1 TUPLE
    NULL
    NEWC
    2 TUPLE
    s1 PUSH
    TLEN
    s0 s3 XCHG
    NEWC
    80 STU
    1 PUSHINT
    s0 PUSH
    2 PUSHINT
    350 PUSHINT
    0 PUSHINT
    s0 PUSH
    <{
      s0 s9 PUSH2
      LESS
    }> PUSHCONT
    <{
      s8 s0 PUSH2
      INDEXVAR
      s0 PUSH
      0 INDEX
      s0 PUSH
      s0 PUSH
      9 LESSINT
      <{
        POW2
      }> PUSHCONT
      <{
        s0 POP
        0 PUSHINT
      }> PUSHCONT
      IFELSE
      s0 PUSH
      0 GTINT
      <{
        s5 s0 PUSH2
        ADD
        10 PUSHPOW2DEC
        GREATER
        <{
          s5 POP
          s0 s7 XCHG
          INC
          0 PUSHINT
          s10 PUSH
          TLEN
          s2 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s8 s5 XCHG2
        }> PUSHCONT
        IF
      }> PUSHCONT
      <{
        s4 PUSH
        INC
        s7 PUSH
        GREATER
        <{
          s4 POP
          s5 POP
          s0 s5 XCHG
          INC
          0 PUSHINT
          4 PUSHINT
          s10 PUSH
          TLEN
          s3 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s7 s6 s6 XCHG3
          s0 s4 XCHG
        }> PUSHCONT
        IF
      }> PUSHCONT
      IFELSE
      s1 PUSH
      9 LESSINT
      <{
        s1 PUSH
        9 EQINT
        <{
          s0 s2 XCHG
          1 INDEX
          s10 s7 PUSH2
          INDEXVAR
          STREF
          s10 s0 s7 XC2PU
          SETINDEXVAR
        }> PUSHCONT
        <{
          s2 POP
          1059 THROW
          s0 s9 XCHG
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      <{
        s0 s2 XCHG
        1 INDEX
        ABS
        s10 s8 PUSH2
        INDEXVAR
        s3 PUSH
        STUX
        s10 s0 s8 XC2PU
        SETINDEXVAR
      }> IFREFELSE
      s9 s8 XCHG2
      4 STU
      s8 PUSH
      0 GTINT
      <{
        3 PUSHINT
        s7 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s7 PUSH
        }> PUSHCONT
        IFELSE
        ROT
        2 STU
        s4 s1 PUXC
        10 STU
        2 STU
        s3 s8 XCHG2
        ADD
      }> <{
        s8 POP
        10 PUSHPOW2DEC
        s5 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s5 PUSH
        }> PUSHCONT
        IFELSE
        s0 s9 XCHG2
        2 STU
        s1 s8 XCHG
        10 STU
        s1 s1 PUXC
        2 STU
        s0 s1 XCHG
        INC
        s7 s0 s7 XCHG3
      }> IFREFELSEREF
      s0 s2 XCHG
      INC
      s6 s7 XCHG_IJ
    }> PUSHREFCONT
    WHILE
    6 BLKDROP
    2 2 BLKDROP2
    s1 PUSH
    1 INDEX
    s2 PUSH
    TLEN
    DEC
    s0 PUSH
    1 GTINT
    <{
      s1 PUSH
      BREFS
      0 EQINT
      <{
        x{} PUSHREF
        x{} PUSHREF
        s0 s3 XCHG2
        STREF
        s1 s2 XCHG
        STREF
      }> PUSHCONT
      <{
        s1 PUSH
        BREFS
        1 EQINT
        <{
          x{} PUSHREF
          ROT
          STREF
          s0 s1 XCHG
        }> PUSHCONT
        IF
        s0 s1 XCHG
      }> PUSHCONT
      IFELSE
      s0 s1 XCHG
    }> IFREF
    350 PUSHINT
    s3 PUSH
    BBITS
    SUB
    s1 s3 XCHG
    STONES
    s0 s1 XCHG
    STBR
    s1 PUSH
    1 EQINT
    <{
      2 1 BLKDROP2
      ENDC
    }> PUSHCONT
    <{
      s0 s1 XCHG
      2 EQINT
      <{
        s0 s1 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        ENDC
      }> PUSHCONT
      IFJMP
      s1 PUSH
      3 INDEX
      ENDC
      s0 s2 XCHG
      2 INDEX
      ENDC
      s0 s1 XCHG
      STREF
      STREF
      ENDC
    }> PUSHCONT
    IFELSE
  }>
  ?fun_102611 PROC:<{
    s0 s1 XCHG
    130966 PUSHINT
    c3 PUSHCTR
    EXECUTE
    s0 s1 XCHG
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      PLDREFVAR
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s0 s1 XCHG
      PLDREFVAR
    }> PUSHCONT
    IFELSE
  }>
  ?fun_102705 PROC:<{
    s0 s1 XCHG
    96786 PUSHINT
    c3 PUSHCTR
    EXECUTE
    s0 s1 XCHG
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      PLDREFVAR
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s0 s1 XCHG
      PLDREFVAR
    }> PUSHCONT
    IFELSE
  }>
  ?fun_104233 PROC:<{
  }>
  ?fun_105830 PROC:<{
    c4 PUSHCTR
    3 PUSHINT
    7 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_107506 PROC:<{
    s1 PUSH
    HASHCU
    68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
    EQUAL
    <{
      DROP2
      NULL
      0 PUSHINT
    }> PUSHCONT
    <{
      s0 s1 XCHG
      8 PUSHPOW2
      DICTUGET
      NULLSWAPIFNOT
    }> PUSHCONT
    IFELSE
  }>
  ?fun_108308 PROC:<{
    s0 PUSH
    5 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    s0 s1 XCHG
    6 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_110883 PROC:<{
    s0 POP
    c4 PUSHCTR
    0 PUSHINT
    1 TUPLE
    1 GETGLOB
    10 INDEX
    0 PUSHINT
    1 GETGLOB
    s0 s1 XCHG
    10 PUSHINT
    SETINDEXVAR
    1 SETGLOB
    ?fun_ref_8de9528bf3b42745 CALLREF
    0 GTINT
    <{
      25942049729782085047030537661247270408528098931008868 PUSHINT
      s1 PUSH
      427070942835 PUSHINT
      7 PUSHINT
      ROT
      2 TUPLE
      1 TUPLE
      NULL
      NEWC
      2 TUPLE
      s1 PUSH
      TLEN
      s0 s3 XCHG
      NEWC
      80 STU
      1 PUSHINT
      s0 PUSH
      2 PUSHINT
      350 PUSHINT
      0 PUSHINT
      s0 PUSH
      <{
        s0 s9 PUSH2
        LESS
      }> PUSHCONT
      <{
        s8 s0 PUSH2
        INDEXVAR
        s0 PUSH
        0 INDEX
        s0 PUSH
        s0 PUSH
        9 LESSINT
        <{
          POW2
        }> PUSHCONT
        <{
          s0 POP
          0 PUSHINT
        }> PUSHCONT
        IFELSE
        s0 PUSH
        0 GTINT
        <{
          s5 s0 PUSH2
          ADD
          10 PUSHPOW2DEC
          GREATER
          <{
            s5 POP
            s0 s7 XCHG
            INC
            0 PUSHINT
            s10 PUSH
            TLEN
            s2 s1 PUXC
            GEQ
            <{
              s0 s10 XCHG
              NEWC
              TPUSH
              s0 s10 XCHG
            }> PUSHCONT
            IF
            s8 s5 XCHG2
          }> PUSHCONT
          IF
        }> PUSHCONT
        <{
          s4 PUSH
          INC
          s7 PUSH
          GREATER
          <{
            s4 POP
            s5 POP
            s0 s5 XCHG
            INC
            0 PUSHINT
            4 PUSHINT
            s10 PUSH
            TLEN
            s3 s1 PUXC
            GEQ
            <{
              s0 s10 XCHG
              NEWC
              TPUSH
              s0 s10 XCHG
            }> PUSHCONT
            IF
            s7 s6 s6 XCHG3
            s0 s4 XCHG
          }> PUSHCONT
          IF
        }> PUSHCONT
        IFELSE
        s1 PUSH
        9 LESSINT
        <{
          s1 PUSH
          9 EQINT
          <{
            s0 s2 XCHG
            1 INDEX
            s10 s7 PUSH2
            INDEXVAR
            STREF
            s10 s0 s7 XC2PU
            SETINDEXVAR
          }> PUSHCONT
          <{
            s2 POP
            1059 THROW
            s0 s9 XCHG
          }> PUSHCONT
          IFELSE
        }> PUSHCONT
        <{
          s0 s2 XCHG
          1 INDEX
          ABS
          s10 s8 PUSH2
          INDEXVAR
          s3 PUSH
          STUX
          s10 s0 s8 XC2PU
          SETINDEXVAR
        }> IFREFELSE
        s9 s8 XCHG2
        4 STU
        s8 PUSH
        0 GTINT
        <{
          3 PUSHINT
          s7 PUSH
          1 EQINT
          <{
            0 PUSHINT
          }> PUSHCONT
          <{
            s7 PUSH
          }> PUSHCONT
          IFELSE
          ROT
          2 STU
          s4 s1 PUXC
          10 STU
          2 STU
          s3 s8 XCHG2
          ADD
        }> <{
          s8 POP
          10 PUSHPOW2DEC
          s5 PUSH
          1 EQINT
          <{
            0 PUSHINT
          }> PUSHCONT
          <{
            s5 PUSH
          }> PUSHCONT
          IFELSE
          s0 s9 XCHG2
          2 STU
          s1 s8 XCHG
          10 STU
          s1 s1 PUXC
          2 STU
          s0 s1 XCHG
          INC
          s7 s0 s7 XCHG3
        }> IFREFELSEREF
        s0 s2 XCHG
        INC
        s6 s7 XCHG_IJ
      }> PUSHREFCONT
      WHILE
      6 BLKDROP
      2 2 BLKDROP2
      s1 PUSH
      1 INDEX
      s2 PUSH
      TLEN
      DEC
      s0 PUSH
      1 GTINT
      <{
        s1 PUSH
        BREFS
        0 EQINT
        <{
          x{} PUSHREF
          x{} PUSHREF
          s0 s3 XCHG2
          STREF
          s1 s2 XCHG
          STREF
        }> PUSHCONT
        <{
          s1 PUSH
          BREFS
          1 EQINT
          <{
            x{} PUSHREF
            ROT
            STREF
            s0 s1 XCHG
          }> PUSHCONT
          IF
          s0 s1 XCHG
        }> PUSHCONT
        IFELSE
        s0 s1 XCHG
      }> IFREF
      350 PUSHINT
      s3 PUSH
      BBITS
      SUB
      s1 s3 XCHG
      STONES
      s0 s1 XCHG
      STBR
      s1 PUSH
      1 EQINT
      <{
        2 1 BLKDROP2
        ENDC
      }> PUSHCONT
      <{
        s0 s1 XCHG
        2 EQINT
        <{
          s0 s1 XCHG
          2 INDEX
          ENDC
          s0 s1 XCHG
          STREF
          ENDC
        }> PUSHCONT
        IFJMP
        s1 PUSH
        3 INDEX
        ENDC
        s0 s2 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        STREF
        ENDC
      }> PUSHCONT
      IFELSE
      s2 s3 XCHG_IJ
      x{} PUSHREF
      ROTREV
      435778055796 PUSHINT
      s0 s3 XCHG
      7850279558805522911016931325 PUSHINT
      NEWC
      134 STU
      216 PUSHINT
      STONES
      s1 s3 XCHG
      256 STU
      STREF
      STREF
      ENDC
      2 TUPLE
      TPUSH
      s2 PUSH
      4 PUSHINT
      8 PUSHPOW2
      s0 s2 XCHG
      CTOS
      s0 s1 XCHG
      18 MULCONST
      80 ADDCONST
      DUP2
      4 ADDCONST
      2 PUSHINT
      SDSUBSTR
      2 PLDU
      s2 s3 PUXC
      6 ADDCONST
      10 PUSHINT
      SDSUBSTR
      10 PLDU
      s1 PUSH
      0 EQINT
      <{
        s1 POP
        s2 PUSH
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      <{
        ROTREV
        PLDREFVAR
        CTOS
        s1 s2 XCPU
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      IFELSE
      ROT
      s0 s3 XCHG
      1 PUSHINT
      8 PUSHPOW2
      s0 s2 XCHG
      CTOS
      s0 s1 XCHG
      18 MULCONST
      80 ADDCONST
      DUP2
      4 ADDCONST
      2 PUSHINT
      SDSUBSTR
      2 PLDU
      s2 s3 PUXC
      6 ADDCONST
      10 PUSHINT
      SDSUBSTR
      10 PLDU
      s1 PUSH
      0 EQINT
      <{
        s1 POP
        s2 PUSH
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      <{
        ROTREV
        PLDREFVAR
        CTOS
        s1 s2 XCPU
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      IFELSE
      s0 s1 s3 XCHG3
      s1 PUSH
      544943221246095313366894 PUSHINT
      s4 s1 BLKSWAP
      5 TUPLE
      TPUSH
    }> <{
      s2 PUSH
      5 PUSHINT
      8 PUSHPOW2
      s0 s2 XCHG
      CTOS
      s0 s1 XCHG
      18 MULCONST
      80 ADDCONST
      DUP2
      4 ADDCONST
      2 PUSHINT
      SDSUBSTR
      2 PLDU
      s2 s3 PUXC
      6 ADDCONST
      10 PUSHINT
      SDSUBSTR
      10 PLDU
      s1 PUSH
      0 EQINT
      <{
        s1 POP
        s2 PUSH
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      <{
        ROTREV
        PLDREFVAR
        CTOS
        s1 s2 XCPU
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      IFELSE
      5 PUSHINT
      s1 s2 XCPU
      ADD
      s2 s4 XCHG_IJ
      ?fun_ref_ec59f05d6c9ac356 CALLREF
      9 PUSHINT
      NOW
      ?fun_ref_ec59f05d6c9ac356 CALLREF
      10 PUSHINT
      s3 PUSH
      ?fun_ref_ec59f05d6c9ac356 CALLREF
      c4 POPCTR
      6641164730824213772039817641279300966331584468661593444 PUSHINT
      s0 s2 XCHG
      427070942835 PUSHINT
      7 PUSHINT
      ROT
      2 TUPLE
      1 TUPLE
      NULL
      NEWC
      2 TUPLE
      s1 PUSH
      TLEN
      s0 s3 XCHG
      NEWC
      80 STU
      1 PUSHINT
      s0 PUSH
      2 PUSHINT
      350 PUSHINT
      0 PUSHINT
      s0 PUSH
      <{
        s0 s9 PUSH2
        LESS
      }> PUSHCONT
      <{
        s8 s0 PUSH2
        INDEXVAR
        s0 PUSH
        0 INDEX
        s0 PUSH
        s0 PUSH
        9 LESSINT
        <{
          POW2
        }> PUSHCONT
        <{
          s0 POP
          0 PUSHINT
        }> PUSHCONT
        IFELSE
        s0 PUSH
        0 GTINT
        <{
          s5 s0 PUSH2
          ADD
          10 PUSHPOW2DEC
          GREATER
          <{
            s5 POP
            s0 s7 XCHG
            INC
            0 PUSHINT
            s10 PUSH
            TLEN
            s2 s1 PUXC
            GEQ
            <{
              s0 s10 XCHG
              NEWC
              TPUSH
              s0 s10 XCHG
            }> PUSHCONT
            IF
            s8 s5 XCHG2
          }> PUSHCONT
          IF
        }> PUSHCONT
        <{
          s4 PUSH
          INC
          s7 PUSH
          GREATER
          <{
            s4 POP
            s5 POP
            s0 s5 XCHG
            INC
            0 PUSHINT
            4 PUSHINT
            s10 PUSH
            TLEN
            s3 s1 PUXC
            GEQ
            <{
              s0 s10 XCHG
              NEWC
              TPUSH
              s0 s10 XCHG
            }> PUSHCONT
            IF
            s7 s6 s6 XCHG3
            s0 s4 XCHG
          }> PUSHCONT
          IF
        }> PUSHCONT
        IFELSE
        s1 PUSH
        9 LESSINT
        <{
          s1 PUSH
          9 EQINT
          <{
            s0 s2 XCHG
            1 INDEX
            s10 s7 PUSH2
            INDEXVAR
            STREF
            s10 s0 s7 XC2PU
            SETINDEXVAR
          }> PUSHCONT
          <{
            s2 POP
            1059 THROW
            s0 s9 XCHG
          }> PUSHCONT
          IFELSE
        }> PUSHCONT
        <{
          s0 s2 XCHG
          1 INDEX
          ABS
          s10 s8 PUSH2
          INDEXVAR
          s3 PUSH
          STUX
          s10 s0 s8 XC2PU
          SETINDEXVAR
        }> IFREFELSE
        s9 s8 XCHG2
        4 STU
        s8 PUSH
        0 GTINT
        <{
          3 PUSHINT
          s7 PUSH
          1 EQINT
          <{
            0 PUSHINT
          }> PUSHCONT
          <{
            s7 PUSH
          }> PUSHCONT
          IFELSE
          ROT
          2 STU
          s4 s1 PUXC
          10 STU
          2 STU
          s3 s8 XCHG2
          ADD
        }> <{
          s8 POP
          10 PUSHPOW2DEC
          s5 PUSH
          1 EQINT
          <{
            0 PUSHINT
          }> PUSHCONT
          <{
            s5 PUSH
          }> PUSHCONT
          IFELSE
          s0 s9 XCHG2
          2 STU
          s1 s8 XCHG
          10 STU
          s1 s1 PUXC
          2 STU
          s0 s1 XCHG
          INC
          s7 s0 s7 XCHG3
        }> IFREFELSEREF
        s0 s2 XCHG
        INC
        s6 s7 XCHG_IJ
      }> PUSHREFCONT
      WHILE
      6 BLKDROP
      2 2 BLKDROP2
      s1 PUSH
      1 INDEX
      s2 PUSH
      TLEN
      DEC
      s0 PUSH
      1 GTINT
      <{
        s1 PUSH
        BREFS
        0 EQINT
        <{
          x{} PUSHREF
          x{} PUSHREF
          s0 s3 XCHG2
          STREF
          s1 s2 XCHG
          STREF
        }> PUSHCONT
        <{
          s1 PUSH
          BREFS
          1 EQINT
          <{
            x{} PUSHREF
            ROT
            STREF
            s0 s1 XCHG
          }> PUSHCONT
          IF
          s0 s1 XCHG
        }> PUSHCONT
        IFELSE
        s0 s1 XCHG
      }> IFREF
      350 PUSHINT
      s3 PUSH
      BBITS
      SUB
      s1 s3 XCHG
      STONES
      s0 s1 XCHG
      STBR
      s1 PUSH
      1 EQINT
      <{
        2 1 BLKDROP2
        ENDC
      }> PUSHCONT
      <{
        s0 s1 XCHG
        2 EQINT
        <{
          s0 s1 XCHG
          2 INDEX
          ENDC
          s0 s1 XCHG
          STREF
          ENDC
        }> PUSHCONT
        IFJMP
        s1 PUSH
        3 INDEX
        ENDC
        s0 s2 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        STREF
        ENDC
      }> PUSHCONT
      IFELSE
      s1 s2 XCHG
      x{} PUSHREF
      ROTREV
      435778055796 PUSHINT
      s0 s3 XCHG
      7850279558805522911016931325 PUSHINT
      NEWC
      134 STU
      216 PUSHINT
      STONES
      s1 s3 XCHG
      256 STU
      STREF
      STREF
      ENDC
      2 TUPLE
      TPUSH
    }> IFREFELSEREF
  }>
  ?fun_111335 PROC:<{
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      PLDREFVAR
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s0 s1 XCHG
      PLDREFVAR
    }> PUSHCONT
    IFELSE
  }>
  ?fun_111835 PROC:<{
    <{
      c2 SAVE
      SAMEALTSAVE
      c4 PUSHCTR
      1 GETGLOB
      9 INDEX
      s0 PUSH
      18406256064951155 PUSHINT
      8 PUSHINT
      ROT
      2 TUPLE
      1 TUPLE
      NULL
      NEWC
      2 TUPLE
      s1 PUSH
      TLEN
      s0 s3 XCHG
      NEWC
      80 STU
      1 PUSHINT
      s0 PUSH
      2 PUSHINT
      350 PUSHINT
      0 PUSHINT
      s0 PUSH
      <{
        s0 s9 PUSH2
        LESS
      }> PUSHCONT
      <{
        s8 s0 PUSH2
        INDEXVAR
        s0 PUSH
        0 INDEX
        s0 PUSH
        s0 PUSH
        9 LESSINT
        <{
          POW2
        }> PUSHCONT
        <{
          s0 POP
          0 PUSHINT
        }> PUSHCONT
        IFELSE
        s0 PUSH
        0 GTINT
        <{
          s5 s0 PUSH2
          ADD
          10 PUSHPOW2DEC
          GREATER
          <{
            s5 POP
            s0 s7 XCHG
            INC
            0 PUSHINT
            s10 PUSH
            TLEN
            s2 s1 PUXC
            GEQ
            <{
              s0 s10 XCHG
              NEWC
              TPUSH
              s0 s10 XCHG
            }> PUSHCONT
            IF
            s8 s5 XCHG2
          }> PUSHCONT
          IF
        }> PUSHCONT
        <{
          s4 PUSH
          INC
          s7 PUSH
          GREATER
          <{
            s4 POP
            s5 POP
            s0 s5 XCHG
            INC
            0 PUSHINT
            4 PUSHINT
            s10 PUSH
            TLEN
            s3 s1 PUXC
            GEQ
            <{
              s0 s10 XCHG
              NEWC
              TPUSH
              s0 s10 XCHG
            }> PUSHCONT
            IF
            s7 s6 s6 XCHG3
            s0 s4 XCHG
          }> PUSHCONT
          IF
        }> PUSHCONT
        IFELSE
        s1 PUSH
        9 LESSINT
        <{
          s1 PUSH
          9 EQINT
          <{
            s0 s2 XCHG
            1 INDEX
            s10 s7 PUSH2
            INDEXVAR
            STREF
            s10 s0 s7 XC2PU
            SETINDEXVAR
          }> PUSHCONT
          <{
            s2 POP
            1059 THROW
            s0 s9 XCHG
          }> PUSHCONT
          IFELSE
        }> PUSHCONT
        <{
          s0 s2 XCHG
          1 INDEX
          ABS
          s10 s8 PUSH2
          INDEXVAR
          s3 PUSH
          STUX
          s10 s0 s8 XC2PU
          SETINDEXVAR
        }> IFREFELSE
        s9 s8 XCHG2
        4 STU
        s8 PUSH
        0 GTINT
        <{
          3 PUSHINT
          s7 PUSH
          1 EQINT
          <{
            0 PUSHINT
          }> PUSHCONT
          <{
            s7 PUSH
          }> PUSHCONT
          IFELSE
          ROT
          2 STU
          s4 s1 PUXC
          10 STU
          2 STU
          s3 s8 XCHG2
          ADD
        }> <{
          s8 POP
          10 PUSHPOW2DEC
          s5 PUSH
          1 EQINT
          <{
            0 PUSHINT
          }> PUSHCONT
          <{
            s5 PUSH
          }> PUSHCONT
          IFELSE
          s0 s9 XCHG2
          2 STU
          s1 s8 XCHG
          10 STU
          s1 s1 PUXC
          2 STU
          s0 s1 XCHG
          INC
          s7 s0 s7 XCHG3
        }> IFREFELSEREF
        s0 s2 XCHG
        INC
        s6 s7 XCHG_IJ
      }> PUSHREFCONT
      WHILE
      6 BLKDROP
      2 2 BLKDROP2
      s1 PUSH
      1 INDEX
      s2 PUSH
      TLEN
      DEC
      s0 PUSH
      1 GTINT
      <{
        s1 PUSH
        BREFS
        0 EQINT
        <{
          x{} PUSHREF
          x{} PUSHREF
          s0 s3 XCHG2
          STREF
          s1 s2 XCHG
          STREF
        }> PUSHCONT
        <{
          s1 PUSH
          BREFS
          1 EQINT
          <{
            x{} PUSHREF
            ROT
            STREF
            s0 s1 XCHG
          }> PUSHCONT
          IF
          s0 s1 XCHG
        }> PUSHCONT
        IFELSE
        s0 s1 XCHG
      }> IFREF
      350 PUSHINT
      s3 PUSH
      BBITS
      SUB
      s1 s3 XCHG
      STONES
      s0 s1 XCHG
      STBR
      s1 PUSH
      1 EQINT
      <{
        2 1 BLKDROP2
        ENDC
      }> PUSHCONT
      <{
        s0 s1 XCHG
        2 EQINT
        <{
          s0 s1 XCHG
          2 INDEX
          ENDC
          s0 s1 XCHG
          STREF
          ENDC
        }> PUSHCONT
        IFJMP
        s1 PUSH
        3 INDEX
        ENDC
        s0 s2 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        STREF
        ENDC
      }> PUSHCONT
      IFELSE
      1 GETGLOB
      1 INDEX
      s3 PUSH
      1 PUSHINT
      8 PUSHPOW2
      s0 s2 XCHG
      CTOS
      s0 s1 XCHG
      18 MULCONST
      80 ADDCONST
      DUP2
      4 ADDCONST
      2 PUSHINT
      SDSUBSTR
      2 PLDU
      s2 s3 PUXC
      6 ADDCONST
      10 PUSHINT
      SDSUBSTR
      10 PLDU
      s1 PUSH
      0 EQINT
      <{
        s1 POP
        s2 PUSH
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      <{
        ROTREV
        PLDREFVAR
        CTOS
        s1 s2 XCPU
        SDSUBSTR
        s0 s1 XCHG
        PLDUX
      }> PUSHCONT
      IFELSE
      EQUAL
      <{
        s3 POP
        s0 s1 XCHG
        4 PUSHINT
        8 PUSHPOW2
        s0 s2 XCHG
        CTOS
        s0 s1 XCHG
        18 MULCONST
        80 ADDCONST
        DUP2
        4 ADDCONST
        2 PUSHINT
        SDSUBSTR
        2 PLDU
        s2 s3 PUXC
        6 ADDCONST
        10 PUSHINT
        SDSUBSTR
        10 PLDU
        s1 PUSH
        0 EQINT
        <{
          s1 POP
          s2 PUSH
          SDSUBSTR
          s0 s1 XCHG
          PLDUX
        }> PUSHCONT
        <{
          ROTREV
          PLDREFVAR
          CTOS
          s1 s2 XCPU
          SDSUBSTR
          s0 s1 XCHG
          PLDUX
        }> PUSHCONT
        IFELSE
        EQUAL
        <{
          s0 POP
          c4 PUSHCTR
          0 PUSHINT
          1 TUPLE
          1 GETGLOB
          10 INDEX
          0 PUSHINT
          1 GETGLOB
          s0 s1 XCHG
          10 PUSHINT
          SETINDEXVAR
          1 SETGLOB
          ?fun_ref_8de9528bf3b42745 CALLREF
          0 GTINT
          <{
            25942049729782085047030537661247270408528098931008868 PUSHINT
            s1 PUSH
            427070942835 PUSHINT
            7 PUSHINT
            ROT
            2 TUPLE
            1 TUPLE
            NULL
            NEWC
            2 TUPLE
            s1 PUSH
            TLEN
            s0 s3 XCHG
            NEWC
            80 STU
            1 PUSHINT
            s0 PUSH
            2 PUSHINT
            350 PUSHINT
            0 PUSHINT
            s0 PUSH
            <{
              s0 s9 PUSH2
              LESS
            }> PUSHCONT
            <{
              s8 s0 PUSH2
              INDEXVAR
              s0 PUSH
              0 INDEX
              s0 PUSH
              s0 PUSH
              9 LESSINT
              <{
                POW2
              }> PUSHCONT
              <{
                s0 POP
                0 PUSHINT
              }> PUSHCONT
              IFELSE
              s0 PUSH
              0 GTINT
              <{
                s5 s0 PUSH2
                ADD
                10 PUSHPOW2DEC
                GREATER
                <{
                  s5 POP
                  s0 s7 XCHG
                  INC
                  0 PUSHINT
                  s10 PUSH
                  TLEN
                  s2 s1 PUXC
                  GEQ
                  <{
                    s0 s10 XCHG
                    NEWC
                    TPUSH
                    s0 s10 XCHG
                  }> PUSHCONT
                  IF
                  s8 s5 XCHG2
                }> PUSHCONT
                IF
              }> PUSHCONT
              <{
                s4 PUSH
                INC
                s7 PUSH
                GREATER
                <{
                  s4 POP
                  s5 POP
                  s0 s5 XCHG
                  INC
                  0 PUSHINT
                  4 PUSHINT
                  s10 PUSH
                  TLEN
                  s3 s1 PUXC
                  GEQ
                  <{
                    s0 s10 XCHG
                    NEWC
                    TPUSH
                    s0 s10 XCHG
                  }> PUSHCONT
                  IF
                  s7 s6 s6 XCHG3
                  s0 s4 XCHG
                }> PUSHCONT
                IF
              }> PUSHCONT
              IFELSE
              s1 PUSH
              9 LESSINT
              <{
                s1 PUSH
                9 EQINT
                <{
                  s0 s2 XCHG
                  1 INDEX
                  s10 s7 PUSH2
                  INDEXVAR
                  STREF
                  s10 s0 s7 XC2PU
                  SETINDEXVAR
                }> PUSHCONT
                <{
                  s2 POP
                  1059 THROW
                  s0 s9 XCHG
                }> PUSHCONT
                IFELSE
              }> PUSHCONT
              <{
                s0 s2 XCHG
                1 INDEX
                ABS
                s10 s8 PUSH2
                INDEXVAR
                s3 PUSH
                STUX
                s10 s0 s8 XC2PU
                SETINDEXVAR
              }> IFREFELSE
              s9 s8 XCHG2
              4 STU
              s8 PUSH
              0 GTINT
              <{
                3 PUSHINT
                s7 PUSH
                1 EQINT
                <{
                  0 PUSHINT
                }> PUSHCONT
                <{
                  s7 PUSH
                }> PUSHCONT
                IFELSE
                ROT
                2 STU
                s4 s1 PUXC
                10 STU
                2 STU
                s3 s8 XCHG2
                ADD
              }> <{
                s8 POP
                10 PUSHPOW2DEC
                s5 PUSH
                1 EQINT
                <{
                  0 PUSHINT
                }> PUSHCONT
                <{
                  s5 PUSH
                }> PUSHCONT
                IFELSE
                s0 s9 XCHG2
                2 STU
                s1 s8 XCHG
                10 STU
                s1 s1 PUXC
                2 STU
                s0 s1 XCHG
                INC
                s7 s0 s7 XCHG3
              }> IFREFELSEREF
              s0 s2 XCHG
              INC
              s6 s7 XCHG_IJ
            }> PUSHREFCONT
            WHILE
            6 BLKDROP
            2 2 BLKDROP2
            s1 PUSH
            1 INDEX
            s2 PUSH
            TLEN
            DEC
            s0 PUSH
            1 GTINT
            <{
              s1 PUSH
              BREFS
              0 EQINT
              <{
                x{} PUSHREF
                x{} PUSHREF
                s0 s3 XCHG2
                STREF
                s1 s2 XCHG
                STREF
              }> PUSHCONT
              <{
                s1 PUSH
                BREFS
                1 EQINT
                <{
                  x{} PUSHREF
                  ROT
                  STREF
                  s0 s1 XCHG
                }> PUSHCONT
                IF
                s0 s1 XCHG
              }> PUSHCONT
              IFELSE
              s0 s1 XCHG
            }> IFREF
            350 PUSHINT
            s3 PUSH
            BBITS
            SUB
            s1 s3 XCHG
            STONES
            s0 s1 XCHG
            STBR
            s1 PUSH
            1 EQINT
            <{
              2 1 BLKDROP2
              ENDC
            }> PUSHCONT
            <{
              s0 s1 XCHG
              2 EQINT
              <{
                s0 s1 XCHG
                2 INDEX
                ENDC
                s0 s1 XCHG
                STREF
                ENDC
              }> PUSHCONT
              IFJMP
              s1 PUSH
              3 INDEX
              ENDC
              s0 s2 XCHG
              2 INDEX
              ENDC
              s0 s1 XCHG
              STREF
              STREF
              ENDC
            }> PUSHCONT
            IFELSE
            s2 s3 XCHG_IJ
            x{} PUSHREF
            ROTREV
            435778055796 PUSHINT
            s0 s3 XCHG
            7850279558805522911016931325 PUSHINT
            NEWC
            134 STU
            216 PUSHINT
            STONES
            s1 s3 XCHG
            256 STU
            STREF
            STREF
            ENDC
            2 TUPLE
            TPUSH
            s2 PUSH
            4 PUSHINT
            8 PUSHPOW2
            s0 s2 XCHG
            CTOS
            s0 s1 XCHG
            18 MULCONST
            80 ADDCONST
            DUP2
            4 ADDCONST
            2 PUSHINT
            SDSUBSTR
            2 PLDU
            s2 s3 PUXC
            6 ADDCONST
            10 PUSHINT
            SDSUBSTR
            10 PLDU
            s1 PUSH
            0 EQINT
            <{
              s1 POP
              s2 PUSH
              SDSUBSTR
              s0 s1 XCHG
              PLDUX
            }> PUSHCONT
            <{
              ROTREV
              PLDREFVAR
              CTOS
              s1 s2 XCPU
              SDSUBSTR
              s0 s1 XCHG
              PLDUX
            }> PUSHCONT
            IFELSE
            ROT
            s0 s3 XCHG
            1 PUSHINT
            8 PUSHPOW2
            s0 s2 XCHG
            CTOS
            s0 s1 XCHG
            18 MULCONST
            80 ADDCONST
            DUP2
            4 ADDCONST
            2 PUSHINT
            SDSUBSTR
            2 PLDU
            s2 s3 PUXC
            6 ADDCONST
            10 PUSHINT
            SDSUBSTR
            10 PLDU
            s1 PUSH
            0 EQINT
            <{
              s1 POP
              s2 PUSH
              SDSUBSTR
              s0 s1 XCHG
              PLDUX
            }> PUSHCONT
            <{
              ROTREV
              PLDREFVAR
              CTOS
              s1 s2 XCPU
              SDSUBSTR
              s0 s1 XCHG
              PLDUX
            }> PUSHCONT
            IFELSE
            s0 s1 s3 XCHG3
            s1 PUSH
            544943221246095313366894 PUSHINT
            s4 s1 BLKSWAP
            5 TUPLE
            TPUSH
          }> <{
            s2 PUSH
            5 PUSHINT
            8 PUSHPOW2
            s0 s2 XCHG
            CTOS
            s0 s1 XCHG
            18 MULCONST
            80 ADDCONST
            DUP2
            4 ADDCONST
            2 PUSHINT
            SDSUBSTR
            2 PLDU
            s2 s3 PUXC
            6 ADDCONST
            10 PUSHINT
            SDSUBSTR
            10 PLDU
            s1 PUSH
            0 EQINT
            <{
              s1 POP
              s2 PUSH
              SDSUBSTR
              s0 s1 XCHG
              PLDUX
            }> PUSHCONT
            <{
              ROTREV
              PLDREFVAR
              CTOS
              s1 s2 XCPU
              SDSUBSTR
              s0 s1 XCHG
              PLDUX
            }> PUSHCONT
            IFELSE
            5 PUSHINT
            s1 s2 XCPU
            ADD
            s2 s4 XCHG_IJ
            ?fun_ref_ec59f05d6c9ac356 CALLREF
            9 PUSHINT
            NOW
            ?fun_ref_ec59f05d6c9ac356 CALLREF
            10 PUSHINT
            s3 PUSH
            ?fun_ref_ec59f05d6c9ac356 CALLREF
            c4 POPCTR
            6641164730824213772039817641279300966331584468661593444 PUSHINT
            s0 s2 XCHG
            427070942835 PUSHINT
            7 PUSHINT
            ROT
            2 TUPLE
            1 TUPLE
            NULL
            NEWC
            2 TUPLE
            s1 PUSH
            TLEN
            s0 s3 XCHG
            NEWC
            80 STU
            1 PUSHINT
            s0 PUSH
            2 PUSHINT
            350 PUSHINT
            0 PUSHINT
            s0 PUSH
            <{
              s0 s9 PUSH2
              LESS
            }> PUSHCONT
            <{
              s8 s0 PUSH2
              INDEXVAR
              s0 PUSH
              0 INDEX
              s0 PUSH
              s0 PUSH
              9 LESSINT
              <{
                POW2
              }> PUSHCONT
              <{
                s0 POP
                0 PUSHINT
              }> PUSHCONT
              IFELSE
              s0 PUSH
              0 GTINT
              <{
                s5 s0 PUSH2
                ADD
                10 PUSHPOW2DEC
                GREATER
                <{
                  s5 POP
                  s0 s7 XCHG
                  INC
                  0 PUSHINT
                  s10 PUSH
                  TLEN
                  s2 s1 PUXC
                  GEQ
                  <{
                    s0 s10 XCHG
                    NEWC
                    TPUSH
                    s0 s10 XCHG
                  }> PUSHCONT
                  IF
                  s8 s5 XCHG2
                }> PUSHCONT
                IF
              }> PUSHCONT
              <{
                s4 PUSH
                INC
                s7 PUSH
                GREATER
                <{
                  s4 POP
                  s5 POP
                  s0 s5 XCHG
                  INC
                  0 PUSHINT
                  4 PUSHINT
                  s10 PUSH
                  TLEN
                  s3 s1 PUXC
                  GEQ
                  <{
                    s0 s10 XCHG
                    NEWC
                    TPUSH
                    s0 s10 XCHG
                  }> PUSHCONT
                  IF
                  s7 s6 s6 XCHG3
                  s0 s4 XCHG
                }> PUSHCONT
                IF
              }> PUSHCONT
              IFELSE
              s1 PUSH
              9 LESSINT
              <{
                s1 PUSH
                9 EQINT
                <{
                  s0 s2 XCHG
                  1 INDEX
                  s10 s7 PUSH2
                  INDEXVAR
                  STREF
                  s10 s0 s7 XC2PU
                  SETINDEXVAR
                }> PUSHCONT
                <{
                  s2 POP
                  1059 THROW
                  s0 s9 XCHG
                }> PUSHCONT
                IFELSE
              }> PUSHCONT
              <{
                s0 s2 XCHG
                1 INDEX
                ABS
                s10 s8 PUSH2
                INDEXVAR
                s3 PUSH
                STUX
                s10 s0 s8 XC2PU
                SETINDEXVAR
              }> IFREFELSE
              s9 s8 XCHG2
              4 STU
              s8 PUSH
              0 GTINT
              <{
                3 PUSHINT
                s7 PUSH
                1 EQINT
                <{
                  0 PUSHINT
                }> PUSHCONT
                <{
                  s7 PUSH
                }> PUSHCONT
                IFELSE
                ROT
                2 STU
                s4 s1 PUXC
                10 STU
                2 STU
                s3 s8 XCHG2
                ADD
              }> <{
                s8 POP
                10 PUSHPOW2DEC
                s5 PUSH
                1 EQINT
                <{
                  0 PUSHINT
                }> PUSHCONT
                <{
                  s5 PUSH
                }> PUSHCONT
                IFELSE
                s0 s9 XCHG2
                2 STU
                s1 s8 XCHG
                10 STU
                s1 s1 PUXC
                2 STU
                s0 s1 XCHG
                INC
                s7 s0 s7 XCHG3
              }> IFREFELSEREF
              s0 s2 XCHG
              INC
              s6 s7 XCHG_IJ
            }> PUSHREFCONT
            WHILE
            6 BLKDROP
            2 2 BLKDROP2
            s1 PUSH
            1 INDEX
            s2 PUSH
            TLEN
            DEC
            s0 PUSH
            1 GTINT
            <{
              s1 PUSH
              BREFS
              0 EQINT
              <{
                x{} PUSHREF
                x{} PUSHREF
                s0 s3 XCHG2
                STREF
                s1 s2 XCHG
                STREF
              }> PUSHCONT
              <{
                s1 PUSH
                BREFS
                1 EQINT
                <{
                  x{} PUSHREF
                  ROT
                  STREF
                  s0 s1 XCHG
                }> PUSHCONT
                IF
                s0 s1 XCHG
              }> PUSHCONT
              IFELSE
              s0 s1 XCHG
            }> IFREF
            350 PUSHINT
            s3 PUSH
            BBITS
            SUB
            s1 s3 XCHG
            STONES
            s0 s1 XCHG
            STBR
            s1 PUSH
            1 EQINT
            <{
              2 1 BLKDROP2
              ENDC
            }> PUSHCONT
            <{
              s0 s1 XCHG
              2 EQINT
              <{
                s0 s1 XCHG
                2 INDEX
                ENDC
                s0 s1 XCHG
                STREF
                ENDC
              }> PUSHCONT
              IFJMP
              s1 PUSH
              3 INDEX
              ENDC
              s0 s2 XCHG
              2 INDEX
              ENDC
              s0 s1 XCHG
              STREF
              STREF
              ENDC
            }> PUSHCONT
            IFELSE
            s1 s2 XCHG
            x{} PUSHREF
            ROTREV
            435778055796 PUSHINT
            s0 s3 XCHG
            7850279558805522911016931325 PUSHINT
            NEWC
            134 STU
            216 PUSHINT
            STONES
            s1 s3 XCHG
            256 STU
            STREF
            STREF
            ENDC
            2 TUPLE
            TPUSH
          }> IFREFELSEREF
        }> PUSHCONT
        IFJMP
        ?fun_ref_cc740b3c607066d4 CALLREF
      }> <{
        s1 POP
        1 GETGLOB
        1 INDEX
        s0 s2 XCHG
        2 PUSHINT
        8 PUSHPOW2
        s0 s2 XCHG
        CTOS
        s0 s1 XCHG
        18 MULCONST
        80 ADDCONST
        DUP2
        4 ADDCONST
        2 PUSHINT
        SDSUBSTR
        2 PLDU
        s2 s3 PUXC
        6 ADDCONST
        10 PUSHINT
        SDSUBSTR
        10 PLDU
        s1 PUSH
        0 EQINT
        <{
          s1 POP
          s2 PUSH
          SDSUBSTR
          s0 s1 XCHG
          PLDUX
        }> PUSHCONT
        <{
          ROTREV
          PLDREFVAR
          CTOS
          s1 s2 XCPU
          SDSUBSTR
          s0 s1 XCHG
          PLDUX
        }> PUSHCONT
        IFELSE
        s1 s2 XCHG
        EQUAL
        <{
          s0 s1 XCHG
          CTOS
          1 LDI
          s0 POP
          <{
            c4 PUSHCTR
            0 PUSHINT
            1 TUPLE
            1 GETGLOB
            10 INDEX
            0 PUSHINT
            1 GETGLOB
            s0 s1 XCHG
            10 PUSHINT
            SETINDEXVAR
            1 SETGLOB
            s0 s3 XCHG
            CTOS
            350 PUSHINT
            8 PUSHPOW2
            SDSUBSTR
            256 PLDU
            s1 s3 BLKSWAP
            s0 s3 XCHG
            1 PUSHINT
            8 PUSHPOW2
            s0 s2 XCHG
            CTOS
            s0 s1 XCHG
            18 MULCONST
            80 ADDCONST
            DUP2
            4 ADDCONST
            2 PUSHINT
            SDSUBSTR
            2 PLDU
            s2 s3 PUXC
            6 ADDCONST
            10 PUSHINT
            SDSUBSTR
            10 PLDU
            s1 PUSH
            0 EQINT
            <{
              s1 POP
              s2 PUSH
              SDSUBSTR
              s0 s1 XCHG
              PLDUX
            }> PUSHCONT
            <{
              ROTREV
              PLDREFVAR
              CTOS
              s1 s2 XCPU
              SDSUBSTR
              s0 s1 XCHG
              PLDUX
            }> PUSHCONT
            IFELSE
            s0 s1 s3 XCHG3
            s1 PUSH
            544943221246095313366894 PUSHINT
            s4 s1 BLKSWAP
            5 TUPLE
            TPUSH
            RETALT
          }> PUSHCONT
          IFJMP
          ?fun_ref_aabe76dab8bd6f6b CALLREF
          RETALT
        }> IFJMPREF
        DROP2
        1325 THROW
        0 PUSHINT
        1 TUPLE
      }> IFREFELSEREF
    }> PUSHCONT
    EXECUTE
  }>
  ?fun_111937 PROC:<{
    8 PUSHINT
    64 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_112653 PROC:<{
    32 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_113356 PROC:<{
    3 PUSHINT
    7 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_114659 PROC:<{
    c4 PUSHCTR
    6 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_116380 PROC:<{
    2 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_116476 PROC:<{
    c4 PUSHCTR
    5 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    ?fun_ref_8de9528bf3b42745 CALLREF
    SUB
  }>
  ?fun_117250 PROC:<{
    1 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    0 NEQINT
  }>
  ?fun_117938 PROC:<{
    0 PUSHINT
    s1 PUSH
    s0 PUSH
    HASHCU
    68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
    EQUAL
    <{
      s0 POP
      -1 PUSHINT
      NULL
    }> PUSHCONT
    <{
      8 PUSHPOW2
      DICTUMIN
      NULLSWAPIFNOT2
      <{
        s0 s1 XCHG
      }> PUSHCONT
      IFJMP
      DROP2
      -1 PUSHINT
      NULL
    }> PUSHCONT
    IFELSE
    s0 POP
    <{
      s0 PUSH
      -1 GTINT
    }> PUSHCONT
    <{
      s2 s1 PUXC
      s1 PUSH
      HASHCU
      68134197439415885698044414435951397869210496020759160419881882418413283430343 PUSHINT
      EQUAL
      <{
        DROP2
        -1 PUSHINT
        NULL
      }> PUSHCONT
      <{
        s0 s1 XCHG
        8 PUSHPOW2
        DICTUGETNEXT
        NULLSWAPIFNOT2
        <{
          s0 s1 XCHG
        }> PUSHCONT
        IFJMP
        DROP2
        -1 PUSHINT
        NULL
      }> PUSHCONT
      IFELSE
      s0 POP
      s0 s1 XCHG
      INC
      s0 s1 XCHG
    }> PUSHREFCONT
    WHILE
    s0 POP
    s1 POP
  }>
  ?fun_119021 PROC:<{
    0 PUSHINT
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      PLDREFVAR
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s0 s1 XCHG
      PLDREFVAR
    }> PUSHCONT
    IFELSE
  }>
  ?fun_119249 PROC:<{
    5 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_120400 PROC:<{
    7850279558805522911016931325 PUSHINT
    NEWC
    134 STU
    216 PUSHINT
    STONES
    s1 s3 XCHG
    256 STU
    STREF
    STREF
    ENDC
  }>
  ?fun_121426 PROC:<{
    7 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_121680 PROC:<{
    4859225033751361390 PUSHINT
    6 PUSHINT
    ROT
    2 TUPLE
    1 TUPLE
    NULL
    NEWC
    2 TUPLE
    s1 PUSH
    TLEN
    s0 s3 XCHG
    NEWC
    80 STU
    1 PUSHINT
    s0 PUSH
    2 PUSHINT
    350 PUSHINT
    0 PUSHINT
    s0 PUSH
    <{
      s0 s9 PUSH2
      LESS
    }> PUSHCONT
    <{
      s8 s0 PUSH2
      INDEXVAR
      s0 PUSH
      0 INDEX
      s0 PUSH
      s0 PUSH
      9 LESSINT
      <{
        POW2
      }> PUSHCONT
      <{
        s0 POP
        0 PUSHINT
      }> PUSHCONT
      IFELSE
      s0 PUSH
      0 GTINT
      <{
        s5 s0 PUSH2
        ADD
        10 PUSHPOW2DEC
        GREATER
        <{
          s5 POP
          s0 s7 XCHG
          INC
          0 PUSHINT
          s10 PUSH
          TLEN
          s2 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s8 s5 XCHG2
        }> PUSHCONT
        IF
      }> PUSHCONT
      <{
        s4 PUSH
        INC
        s7 PUSH
        GREATER
        <{
          s4 POP
          s5 POP
          s0 s5 XCHG
          INC
          0 PUSHINT
          4 PUSHINT
          s10 PUSH
          TLEN
          s3 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s7 s6 s6 XCHG3
          s0 s4 XCHG
        }> PUSHCONT
        IF
      }> PUSHCONT
      IFELSE
      s1 PUSH
      9 LESSINT
      <{
        s1 PUSH
        9 EQINT
        <{
          s0 s2 XCHG
          1 INDEX
          s10 s7 PUSH2
          INDEXVAR
          STREF
          s10 s0 s7 XC2PU
          SETINDEXVAR
        }> PUSHCONT
        <{
          s2 POP
          1059 THROW
          s0 s9 XCHG
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      <{
        s0 s2 XCHG
        1 INDEX
        ABS
        s10 s8 PUSH2
        INDEXVAR
        s3 PUSH
        STUX
        s10 s0 s8 XC2PU
        SETINDEXVAR
      }> IFREFELSE
      s9 s8 XCHG2
      4 STU
      s8 PUSH
      0 GTINT
      <{
        3 PUSHINT
        s7 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s7 PUSH
        }> PUSHCONT
        IFELSE
        ROT
        2 STU
        s4 s1 PUXC
        10 STU
        2 STU
        s3 s8 XCHG2
        ADD
      }> <{
        s8 POP
        10 PUSHPOW2DEC
        s5 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s5 PUSH
        }> PUSHCONT
        IFELSE
        s0 s9 XCHG2
        2 STU
        s1 s8 XCHG
        10 STU
        s1 s1 PUXC
        2 STU
        s0 s1 XCHG
        INC
        s7 s0 s7 XCHG3
      }> IFREFELSEREF
      s0 s2 XCHG
      INC
      s6 s7 XCHG_IJ
    }> PUSHREFCONT
    WHILE
    6 BLKDROP
    2 2 BLKDROP2
    s1 PUSH
    1 INDEX
    s2 PUSH
    TLEN
    DEC
    s0 PUSH
    1 GTINT
    <{
      s1 PUSH
      BREFS
      0 EQINT
      <{
        x{} PUSHREF
        x{} PUSHREF
        s0 s3 XCHG2
        STREF
        s1 s2 XCHG
        STREF
      }> PUSHCONT
      <{
        s1 PUSH
        BREFS
        1 EQINT
        <{
          x{} PUSHREF
          ROT
          STREF
          s0 s1 XCHG
        }> PUSHCONT
        IF
        s0 s1 XCHG
      }> PUSHCONT
      IFELSE
      s0 s1 XCHG
    }> IFREF
    350 PUSHINT
    s3 PUSH
    BBITS
    SUB
    s1 s3 XCHG
    STONES
    s0 s1 XCHG
    STBR
    s1 PUSH
    1 EQINT
    <{
      2 1 BLKDROP2
      ENDC
    }> PUSHCONT
    <{
      s0 s1 XCHG
      2 EQINT
      <{
        s0 s1 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        ENDC
      }> PUSHCONT
      IFJMP
      s1 PUSH
      3 INDEX
      ENDC
      s0 s2 XCHG
      2 INDEX
      ENDC
      s0 s1 XCHG
      STREF
      STREF
      ENDC
    }> PUSHCONT
    IFELSE
  }>
  ?fun_124063 PROC:<{
    1814802986876646945381 PUSHINT
    8 PUSHINT
    ROT
    2 TUPLE
    0 PUSHINT
    0 PUSHINT
    2 TUPLE
    0 PUSHINT
    0 PUSHINT
    2 TUPLE
    9 PUSHINT
    x{} PUSHREF
    2 TUPLE
    4 TUPLE
    NULL
    NEWC
    2 TUPLE
    s1 PUSH
    TLEN
    s0 s3 XCHG
    NEWC
    80 STU
    1 PUSHINT
    s0 PUSH
    2 PUSHINT
    350 PUSHINT
    0 PUSHINT
    s0 PUSH
    <{
      s0 s9 PUSH2
      LESS
    }> PUSHCONT
    <{
      s8 s0 PUSH2
      INDEXVAR
      s0 PUSH
      0 INDEX
      s0 PUSH
      s0 PUSH
      9 LESSINT
      <{
        POW2
      }> PUSHCONT
      <{
        s0 POP
        0 PUSHINT
      }> PUSHCONT
      IFELSE
      s0 PUSH
      0 GTINT
      <{
        s5 s0 PUSH2
        ADD
        10 PUSHPOW2DEC
        GREATER
        <{
          s5 POP
          s0 s7 XCHG
          INC
          0 PUSHINT
          s10 PUSH
          TLEN
          s2 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s8 s5 XCHG2
        }> PUSHCONT
        IF
      }> PUSHCONT
      <{
        s4 PUSH
        INC
        s7 PUSH
        GREATER
        <{
          s4 POP
          s5 POP
          s0 s5 XCHG
          INC
          0 PUSHINT
          4 PUSHINT
          s10 PUSH
          TLEN
          s3 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s7 s6 s6 XCHG3
          s0 s4 XCHG
        }> PUSHCONT
        IF
      }> PUSHCONT
      IFELSE
      s1 PUSH
      9 LESSINT
      <{
        s1 PUSH
        9 EQINT
        <{
          s0 s2 XCHG
          1 INDEX
          s10 s7 PUSH2
          INDEXVAR
          STREF
          s10 s0 s7 XC2PU
          SETINDEXVAR
        }> PUSHCONT
        <{
          s2 POP
          1059 THROW
          s0 s9 XCHG
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      <{
        s0 s2 XCHG
        1 INDEX
        ABS
        s10 s8 PUSH2
        INDEXVAR
        s3 PUSH
        STUX
        s10 s0 s8 XC2PU
        SETINDEXVAR
      }> IFREFELSE
      s9 s8 XCHG2
      4 STU
      s8 PUSH
      0 GTINT
      <{
        3 PUSHINT
        s7 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s7 PUSH
        }> PUSHCONT
        IFELSE
        ROT
        2 STU
        s4 s1 PUXC
        10 STU
        2 STU
        s3 s8 XCHG2
        ADD
      }> <{
        s8 POP
        10 PUSHPOW2DEC
        s5 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s5 PUSH
        }> PUSHCONT
        IFELSE
        s0 s9 XCHG2
        2 STU
        s1 s8 XCHG
        10 STU
        s1 s1 PUXC
        2 STU
        s0 s1 XCHG
        INC
        s7 s0 s7 XCHG3
      }> IFREFELSEREF
      s0 s2 XCHG
      INC
      s6 s7 XCHG_IJ
    }> PUSHREFCONT
    WHILE
    6 BLKDROP
    2 2 BLKDROP2
    s1 PUSH
    1 INDEX
    s2 PUSH
    TLEN
    DEC
    s0 PUSH
    1 GTINT
    <{
      s1 PUSH
      BREFS
      0 EQINT
      <{
        x{} PUSHREF
        x{} PUSHREF
        s0 s3 XCHG2
        STREF
        s1 s2 XCHG
        STREF
      }> PUSHCONT
      <{
        s1 PUSH
        BREFS
        1 EQINT
        <{
          x{} PUSHREF
          ROT
          STREF
          s0 s1 XCHG
        }> PUSHCONT
        IF
        s0 s1 XCHG
      }> PUSHCONT
      IFELSE
      s0 s1 XCHG
    }> IFREF
    350 PUSHINT
    s3 PUSH
    BBITS
    SUB
    s1 s3 XCHG
    STONES
    s0 s1 XCHG
    STBR
    s1 PUSH
    1 EQINT
    <{
      2 1 BLKDROP2
      ENDC
    }> PUSHCONT
    <{
      s0 s1 XCHG
      2 EQINT
      <{
        s0 s1 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        ENDC
      }> PUSHCONT
      IFJMP
      s1 PUSH
      3 INDEX
      ENDC
      s0 s2 XCHG
      2 INDEX
      ENDC
      s0 s1 XCHG
      STREF
      STREF
      ENDC
    }> PUSHCONT
    IFELSE
  }>
  ?fun_125767 PROC:<{
    c4 PUSHCTR
  }>
  ?fun_126275 PROC:<{
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      PLDREFVAR
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s0 s1 XCHG
      PLDREFVAR
    }> PUSHCONT
    IFELSE
  }>
  ?fun_126862 PROC:<{
    24318394545827431 PUSHINT
    6 PUSHINT
    ROT
    2 TUPLE
    1 TUPLE
    NULL
    NEWC
    2 TUPLE
    s1 PUSH
    TLEN
    s0 s3 XCHG
    NEWC
    80 STU
    1 PUSHINT
    s0 PUSH
    2 PUSHINT
    350 PUSHINT
    0 PUSHINT
    s0 PUSH
    <{
      s0 s9 PUSH2
      LESS
    }> PUSHCONT
    <{
      s8 s0 PUSH2
      INDEXVAR
      s0 PUSH
      0 INDEX
      s0 PUSH
      s0 PUSH
      9 LESSINT
      <{
        POW2
      }> PUSHCONT
      <{
        s0 POP
        0 PUSHINT
      }> PUSHCONT
      IFELSE
      s0 PUSH
      0 GTINT
      <{
        s5 s0 PUSH2
        ADD
        10 PUSHPOW2DEC
        GREATER
        <{
          s5 POP
          s0 s7 XCHG
          INC
          0 PUSHINT
          s10 PUSH
          TLEN
          s2 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s8 s5 XCHG2
        }> PUSHCONT
        IF
      }> PUSHCONT
      <{
        s4 PUSH
        INC
        s7 PUSH
        GREATER
        <{
          s4 POP
          s5 POP
          s0 s5 XCHG
          INC
          0 PUSHINT
          4 PUSHINT
          s10 PUSH
          TLEN
          s3 s1 PUXC
          GEQ
          <{
            s0 s10 XCHG
            NEWC
            TPUSH
            s0 s10 XCHG
          }> PUSHCONT
          IF
          s7 s6 s6 XCHG3
          s0 s4 XCHG
        }> PUSHCONT
        IF
      }> PUSHCONT
      IFELSE
      s1 PUSH
      9 LESSINT
      <{
        s1 PUSH
        9 EQINT
        <{
          s0 s2 XCHG
          1 INDEX
          s10 s7 PUSH2
          INDEXVAR
          STREF
          s10 s0 s7 XC2PU
          SETINDEXVAR
        }> PUSHCONT
        <{
          s2 POP
          1059 THROW
          s0 s9 XCHG
        }> PUSHCONT
        IFELSE
      }> PUSHCONT
      <{
        s0 s2 XCHG
        1 INDEX
        ABS
        s10 s8 PUSH2
        INDEXVAR
        s3 PUSH
        STUX
        s10 s0 s8 XC2PU
        SETINDEXVAR
      }> IFREFELSE
      s9 s8 XCHG2
      4 STU
      s8 PUSH
      0 GTINT
      <{
        3 PUSHINT
        s7 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s7 PUSH
        }> PUSHCONT
        IFELSE
        ROT
        2 STU
        s4 s1 PUXC
        10 STU
        2 STU
        s3 s8 XCHG2
        ADD
      }> <{
        s8 POP
        10 PUSHPOW2DEC
        s5 PUSH
        1 EQINT
        <{
          0 PUSHINT
        }> PUSHCONT
        <{
          s5 PUSH
        }> PUSHCONT
        IFELSE
        s0 s9 XCHG2
        2 STU
        s1 s8 XCHG
        10 STU
        s1 s1 PUXC
        2 STU
        s0 s1 XCHG
        INC
        s7 s0 s7 XCHG3
      }> IFREFELSEREF
      s0 s2 XCHG
      INC
      s6 s7 XCHG_IJ
    }> PUSHREFCONT
    WHILE
    6 BLKDROP
    2 2 BLKDROP2
    s1 PUSH
    1 INDEX
    s2 PUSH
    TLEN
    DEC
    s0 PUSH
    1 GTINT
    <{
      s1 PUSH
      BREFS
      0 EQINT
      <{
        x{} PUSHREF
        x{} PUSHREF
        s0 s3 XCHG2
        STREF
        s1 s2 XCHG
        STREF
      }> PUSHCONT
      <{
        s1 PUSH
        BREFS
        1 EQINT
        <{
          x{} PUSHREF
          ROT
          STREF
          s0 s1 XCHG
        }> PUSHCONT
        IF
        s0 s1 XCHG
      }> PUSHCONT
      IFELSE
      s0 s1 XCHG
    }> IFREF
    350 PUSHINT
    s3 PUSH
    BBITS
    SUB
    s1 s3 XCHG
    STONES
    s0 s1 XCHG
    STBR
    s1 PUSH
    1 EQINT
    <{
      2 1 BLKDROP2
      ENDC
    }> PUSHCONT
    <{
      s0 s1 XCHG
      2 EQINT
      <{
        s0 s1 XCHG
        2 INDEX
        ENDC
        s0 s1 XCHG
        STREF
        ENDC
      }> PUSHCONT
      IFJMP
      s1 PUSH
      3 INDEX
      ENDC
      s0 s2 XCHG
      2 INDEX
      ENDC
      s0 s1 XCHG
      STREF
      STREF
      ENDC
    }> PUSHCONT
    IFELSE
  }>
  ?fun_128150 PROC:<{
    c4 PUSHCTR
    s0 s1 PUSH2
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    4 PUSHINT
    SDSUBSTR
    4 PLDU
    s0 PUSH
    9 LESSINT
    <{
      POW2
    }> PUSHCONT
    <{
      s0 POP
      0 PUSHINT
    }> PUSHCONT
    IFELSE
    s1 s2 XCHG
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_128730 PROC:<{
    1 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_129371 PROC:<{
    64 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
  }>
  ?fun_129818 PROC:<{
    x{} PUSHREF
  }>
  ?fun_130966 PROC:<{
    c4 PUSHCTR
    s0 s1 XCHG
    s0 s1 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      PLDREFVAR
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s0 s1 XCHG
      PLDREFVAR
    }> PUSHCONT
    IFELSE
  }>
  ?fun_ref_29ce7f6238ca878a PROCREF:<{
    s5 PUSH
    3 PUSHINT
    7 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    s0 s6 XCHG
    2 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    s3 s1 BLKSWAP
    NULL
    s0 s1 XCHG
    1 GETGLOB
    4 INDEX
    s0 s1 XCHG
    NEWC
    32 STU
    64 STU
    s0 s3 XCHG2
    STGRAMS
    64 STU
    x{2_} PUSHSLICE
    STSLICER
    x{2_} PUSHSLICE
    STSLICER
    0 PUSHINT
    STGRAMS
    STDICT
    ENDC
    1680571655 PUSHINT
    1 GETGLOB
    4 INDEX
    s0 s1 XCHG
    NEWC
    32 STU
    64 STU
    s0 s3 XCHG
    0 PUSHINT
    4 PUSHINT
    NEWC
    3 STU
    8 STI
    256 STU
    ENDC
    CTOS
    s1 s3 XCHG
    STSLICER
    s0 s4 XCHG2
    STGRAMS
    STREF
    ENDC
    s1 s2 XCHG
    32195312204475500 PUSHINT
    ROTREV
    3 TUPLE
    TPUSH
  }>
  ?fun_ref_2ec803d30dedfe82 PROCREF:<{
    c4 PUSHCTR
    5 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    ?fun_ref_8de9528bf3b42745 CALLREF
    SUB
    c4 PUSHCTR
    6 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    s0 PUSH
    0 EQINT
    s2 PUSH
    0 EQINT
    OR
    <{
      DROP2
    }> PUSHCONT
    <{
      s0 s1 XCHG
      MULDIV
    }> PUSHCONT
    IFELSE
  }>
  ?fun_ref_7a8d9a5395189798 PROCREF:<{
    s1 PUSH
    CTOS
    4 LDU
    0 PUSHINT
    s0 s2 XCHG
    1 PUSHINT
    AND
    <{
      s1 POP
      -1 PUSHINT
      s0 s2 XCHG
      32 PUSHINT
      SDSKIPFIRST
      ROTREV
    }> PUSHCONT
    IF
    -1 PUSHINT
    s0 PUSH
    0 PUSHINT
    NULL
    s0 s4 XCHG
    LDMSGADDR
    LDMSGADDR
    s1 POP
    LDGRAMS
    s1 POP
    SKIPDICT
    LDGRAMS
    s1 POP
    s0 s1 XCHG
    REWRITESTDADDR
    s0 s1 XCHG
    0 NEQINT
    2047 THROWIF
    s0 s7 PUSH2
    SEMPTY
    <{
      3 3 BLKDROP2
      s0 s5 XCHG
      32 LDU
      64 LDU
      LDGRAMS
      s0 PUSH
      SBITS
      267 PUSHINT
      GEQ
      <{
        s8 POP
        s7 PUSH
        11 PUSHINT
        8 PUSHPOW2
        SDSUBSTR
        256 PLDU
        s0 s8 XCHG
      }> PUSHCONT
      IF
      s0 PUSH
      SREMPTY
      <{
        s6 POP
        s5 PUSH
        0 PLDREFIDX
        s0 s6 XCHG
      }> PUSHCONT
      IFNOT
      s0 s8 XCHG
      s3 s5 XCHG_IJ
      s4 s3 s0 XCHG3
      s1 s2 XCHG
    }> PUSHCONT
    IFNOT
    s0 s2 XCHG
    LDGRAMS
    s0 POP
    3 PUSHINT
    1 MULRSHIFT
    s7 s11 XCHG_IJ
    s10 s9 XCHG2
    s5 s8 XCHG_IJ
    s4 s7 XCHG_IJ
    s4 s6 XCHG_IJ
    s3 s3 XCHG2
    s0 s5 XCHG
    s0 s4 XCHG
    12 TUPLE
    1 SETGLOB
  }>
  ?fun_ref_8de9528bf3b42745 PROCREF:<{
    c4 PUSHCTR
    s0 PUSH
    9 PUSHINT
    64 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    s1 PUSH
    10 PUSHINT
    7 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    s0 s2 XCHG
    11 PUSHINT
    64 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    NOW
    ROT
    SUB
    s0 s1 PUSH2
    GEQ
    <{
      3 BLKDROP
      0 PUSHINT
    }> PUSHCONT
    <{
      s1 s1 PUXC
      SUB
      ROT
      MUL
      s0 s1 XCHG
      DIV
    }> PUSHCONT
    IFELSE
  }>
  ?fun_ref_99efc4fa43084414 PROCREF:<{
    c4 PUSHCTR
    5 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    ?fun_ref_8de9528bf3b42745 CALLREF
    SUB
    c4 PUSHCTR
    6 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    s0 PUSH
    0 EQINT
    s2 PUSH
    0 EQINT
    OR
    <{
      3 BLKDROP
      0 PUSHINT
    }> PUSHCONT
    <{
      MULDIV
    }> PUSHCONT
    IFELSE
  }>
  ?fun_ref_aabe76dab8bd6f6b PROCREF:<{
    c4 PUSHCTR
    0 PUSHINT
    1 TUPLE
    1 GETGLOB
    10 INDEX
    0 PUSHINT
    1 GETGLOB
    s0 s1 XCHG
    10 PUSHINT
    SETINDEXVAR
    1 SETGLOB
    s0 PUSH
    ?fun_ref_99efc4fa43084414 CALLREF
    s3 PUSH
    s0 PUSH
    5 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    s0 s1 XCHG
    6 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    5 PUSHINT
    s2 s3 XCPU
    SUB
    s6 s4 s4 PUXC2
    ?fun_ref_ec59f05d6c9ac356 CALLREF
    s0 s1 XCHG
    6 PUSHINT
    s0 s4 XCHG
    SUB
    s1 s3 s0 XCHG3
    ?fun_ref_ec59f05d6c9ac356 CALLREF
    c4 POPCTR
    s0 s3 XCHG
    CTOS
    350 PUSHINT
    8 PUSHPOW2
    SDSUBSTR
    256 PLDU
    NOW
    s3 PUSH
    8 PUSHINT
    64 PUSHINT
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    ADD
    s3 s4 XCHG_IJ
    s1 s3 s0 XCHG3
    2992127701 PUSHINT
    ?fun_ref_29ce7f6238ca878a CALLREF
  }>
  ?fun_ref_cc740b3c607066d4 PROCREF:<{
    c4 PUSHCTR
    0 PUSHINT
    1 TUPLE
    s1 PUSH
    s0 PUSH
    5 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    s0 s1 XCHG
    6 PUSHINT
    8 PUSHPOW2
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s3 PUXC
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s1 PUSH
    0 EQINT
    <{
      s1 POP
      s2 PUSH
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    <{
      ROTREV
      PLDREFVAR
      CTOS
      s1 s2 XCPU
      SDSUBSTR
      s0 s1 XCHG
      PLDUX
    }> PUSHCONT
    IFELSE
    1 GETGLOB
    10 INDEX
    0 PUSHINT
    1 GETGLOB
    s0 s1 XCHG
    10 PUSHINT
    SETINDEXVAR
    1 SETGLOB
    s0 PUSH
    ?fun_ref_2ec803d30dedfe82 CALLREF
    s0 s3 XCHG
    5 PUSHINT
    s0 s2 XCHG
    ADD
    s5 PUSH
    ROTREV
    ?fun_ref_ec59f05d6c9ac356 CALLREF
    6 PUSHINT
    s2 s3 XCPU
    ADD
    s1 s2 XCHG
    ?fun_ref_ec59f05d6c9ac356 CALLREF
    c4 POPCTR
    s0 s3 XCHG
    CTOS
    350 PUSHINT
    8 PUSHPOW2
    SDSUBSTR
    256 PLDU
    s1 s3 BLKSWAP
    0 PUSHINT
    395134233 PUSHINT
    ?fun_ref_29ce7f6238ca878a CALLREF
  }>
  ?fun_ref_ec59f05d6c9ac356 PROCREF:<{
    s0 s2 XCHG
    CTOS
    s0 s1 XCHG
    18 MULCONST
    80 ADDCONST
    DUP2
    4 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s2 s1 PUSH2
    4 PUSHINT
    SDSUBSTR
    4 PLDU
    OVER2
    6 ADDCONST
    10 PUSHINT
    SDSUBSTR
    10 PLDU
    s4 s5 PUXC
    16 ADDCONST
    2 PUSHINT
    SDSUBSTR
    2 PLDU
    s1 PUSH
    s0 PUSH
    9 LESSINT
    <{
      POW2
    }> PUSHCONT
    <{
      s0 POP
      0 PUSHINT
    }> PUSHCONT
    IFELSE
    s3 PUSH
    0 EQINT
    <{
      s5 PUSH
    }> PUSHCONT
    <{
      s5 s3 PUSH2
      PLDREFVAR
      CTOS
    }> PUSHCONT
    IFELSE
    s1 PUSH
    0 NEQINT
    <{
      4 PUSHINT
      s3 POP
    }> PUSHCONT
    IF
    NEWC
    s1 PUSH
    SBITS
    s7 PUSH
    MIN
    s2 PUSH
    SREFS
    s5 s1 PUXC
    MIN
    s3 PUSH
    ROTREV
    SCUTFIRST
    STSLICER
    s0 s4 XCHG
    9 EQINT
    <{
      s2 POP
      s0 s6 XCHG
      ABS
      s0 s2 s6 XC2PU
      STUX
      s3 s5 XCHG2
      ADD
      s2 PUSH
      SREFS
      s2 s3 XCHG_IJ
      SSKIPFIRST
      s1 s3 XCHG
      STSLICER
    }> PUSHCONT
    <{
      s1 POP
      s4 POP
      s5 s5 XCHG2
      STREF
      0 PUSHINT
      s3 PUSH
      SREFS
      s0 s6 XCHG2
      SUB
      DEC
      s3 s5 s0 XCHG3
      SCUTLAST
      s1 s3 XCHG
      STSLICER
    }> IFREFELSE
    s2 PUSH
    0 GTINT
    <{
      ENDC
      NEWC
      s2 PUSH
      SBITS
      s3 s6 s3 PU2XC
      SCUTFIRST
      STSLICER
      STREF
      0 PUSHINT
      s2 PUSH
      SREFS
      s0 s4 XCHG2
      SUB
      DEC
      s1 s3 XCHG
      SCUTLAST
      STSLICER
      ENDC
    }> PUSHCONT
    <{
      2 1 BLKDROP2
      ENDC
    }> PUSHCONT
    IFELSE
  }>
}END>c