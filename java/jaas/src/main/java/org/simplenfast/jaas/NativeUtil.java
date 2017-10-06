/*
 * The MIT License
 *
 * Copyright 2017 Rakesh Didwania.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
package org.simplenfast.jaas;

import javax.security.auth.login.LoginException;

public class NativeUtil
{
	/**
	 * Creates LoginException from NativeException.
	 * 
	 * @param ex NativeException
	 * @return LoginException
	 */
	static LoginException NativeToLoginException(NativeException ex)	
	{
		StringBuilder strBldr = new StringBuilder(ex.getMessage());

		if (ex.getErrorString() != null) {
			strBldr.append(": ");
			strBldr.append(ex.getErrorString());
			if (ex.getErrorCode() != -1) {
				strBldr.append(" (");
				strBldr.append(ex.getErrorCode());
				strBldr.append(")");
			}
		} else if (ex.getErrorCode() != -1) {
				strBldr.append(": ");
				strBldr.append(ex.getErrorCode());
		}

		LoginException loginEx = new LoginException(strBldr.toString());
		loginEx.initCause(ex);

		return loginEx;
	}
}