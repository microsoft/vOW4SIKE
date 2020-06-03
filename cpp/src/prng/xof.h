#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// todo: need to interface-ace? not really I think, it's a single stateless function
// todo: check and add a requirement for in-place computation safety
void XOF(unsigned char *output, unsigned char *input, unsigned long nbytes_output, unsigned long nbytes_input, unsigned long salt);

#ifdef __cplusplus
}
#endif