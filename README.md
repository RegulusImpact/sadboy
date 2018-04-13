# Testing:

## BLARGG'S CPU Intruction Tests:

### 03-op sp,hl - _Failed_
```
03-op sp,hl

88 88
Failed
```
### 04-op r,imm - _Failed_
```
04-op r,imm

EE EE
Failed
```

### 09-op r,r - _Failed_
```
09-op r,r

88 99 AA BB CC DD FF 88 99 AA BB CC DD FF BB 00 BB 11 BB 22 BB 33 BB 44 BB 55 BB 77
Failed

```

### 11-op a,(hl) - _Failed_
```
11-op a,(hl)

EE 66 EE BB 66 BB 66
Failed
```


Git Submodule Reference:
https://git-scm.com/book/en/v2/Git-Tools-Submodules
