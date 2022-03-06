
		section	code,code


		XDEF	_EmbeddedTestRunner_runTestAtAddress
_EmbeddedTestRunner_runTestAtAddress:

		jsr (a0)
		rts