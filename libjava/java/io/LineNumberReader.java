/* LineNumberReader.java -- A character input stream which counts line numbers
   Copyright (C) 1998, 1999, 2001, 2003 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

package java.io;

/**
 * @author Per Bothner <bothner@cygnus.com>
 * @date April 22, 1998.  
 */
/* Written using "Java Class Libraries", 2nd edition, plus online
 * API docs for JDK 1.2 beta from http://www.javasoft.com.
 * Status:  Believed complete and correct.
 *
 * This implementation has the feature that if '\r' is read, it
 * does not look for a '\n', but immediately returns '\n'.
 * On the next read(), if a '\n' is read, it is skipped.
 * This has the advantage that we do not read (and hang) unnecessarily.
 *
 * This implementation is also minimal in the number of fields it uses.
 */

public class LineNumberReader extends BufferedReader
{
  /** The current line number. */
  int lineNumber;

  public LineNumberReader(Reader in)
  {
    super(in, 8192);
  }

  public LineNumberReader(Reader in, int size)
  {
    super(in, size);
  }

  public int getLineNumber()
  {
    return lineNumber;
  }

  public void setLineNumber(int lineNumber)
  {
    this.lineNumber = lineNumber;
  }

  private static int countLines (char[] buffer, int off, int len)
  {
    int count = 0;
    char prev = '\0';
    for (int i = 0;  i < len;  i++)
      {
        char ch = buffer[i+off];
        if ((ch == '\n' && prev != '\r') || ch == '\r')
          count++;
        prev = ch;
      }
    return count;
  }

  public void mark(int readLimit) throws IOException
  {
    synchronized (lock)
      {
	// This is basically the same as BufferedReader.mark.
	// However, if the previous character was a '\r', we need to
	// save that 'r', in case the next character is a '\n'.
	if (pos + readLimit > limit)
	  {
	    int saveCR = (pos > 0 && buffer[pos-1] == '\r') ? 1 : 0;
	    char[] old_buffer = buffer;
	    if (readLimit > limit)
	      buffer = new char[saveCR + readLimit];
	    int copy_start = pos - saveCR;
	    limit -= copy_start;
	    System.arraycopy(old_buffer, copy_start, buffer, 0, limit);
	    pos = saveCR;
	  }
	markPos = pos;
      }
  }

  public void reset() throws IOException
  {
    synchronized (lock)
      {
	if (markPos < 0)
	  throw new IOException("mark never set or invalidated");
	if (markPos > 0 && pos > markPos && buffer[markPos-1] == '\r'
	    && buffer[markPos] == '\n')
	  lineNumber--;
	lineNumber -= countLines(buffer, markPos, pos - markPos);
	pos = markPos;
      }
  }

  public int read() throws IOException
  {
    synchronized (lock)
      {
	skipRedundantLF();
	if (pos >= limit)
	  {
	    if (markPos >= 0 && limit == buffer.length)
	      markPos = -1;
	    if (markPos <= 0)
	      pos = limit = 0;
	    int count = in.read(buffer, limit, buffer.length - limit);
	    if (count <= 0)
	      return -1;
	    limit += count;
	  }
	char ch = buffer[pos++];
	if (ch == '\r' || ch == '\n')
	  {
	    lineNumber++;
	    return '\n';
	  }
	return (int) ch;
      }
  }

  public int read(char[] buf, int offset, int count) throws IOException
  {
    if (count <= 0)
      {
	if (count < 0)
	  throw new IndexOutOfBoundsException();
	return 0;
      }
    synchronized (lock)
      {
	int first = read();
	if (first < 0)
	  return -1;
	int start_offset = offset;
	buf[offset++] = (char) first;
	if (buffer[pos-1] == '\r' && pos < limit && buffer[pos] == '\n')
	  pos++;
	count--;
	while (count-- > 0 && pos < limit)
	  {
	    char ch = buffer[pos++];
	    if (ch == '\r')
	      {
		lineNumber++;
		ch = '\n';
		if (pos < limit && buffer[pos] == '\n')
		  pos++;
	      }
	    else if (ch == '\n')
	      lineNumber++;
	    buf[offset++] = ch;
	  }
	return offset - start_offset;
      }
  }

  private void skipRedundantLF() throws IOException
  {
    if (pos > 0 && buffer[pos-1] == '\r')
      {
	if (pos < limit)
	  { // fast case
	    if (buffer[pos] == '\n')
	      pos++;
	  }
	else
	  { // use read() to deal with the general case.
	    // Set pos and limit to zero to avoid infinite recursion in read.
	    // May need to invalidate markPos if we've exceeded the buffer.  
	    if (pos >= buffer.length)
	      markPos = -1;
	    pos = limit = 0;
	    int ch = read();
	    if (ch >= 0 && ch != '\n')
	      pos--;
	  }
      }
  }

  public String readLine() throws IOException
  {
    // BufferedReader.readLine already does this.  Shouldn't need to keep
    // track of newlines (since the read method deals with this for us).
    // But if the buffer is large, we may not call the read method at all
    // and super.readLine can't increment lineNumber itself.
    // Though it may seem kludgy, the safest thing to do is to save off
    // lineNumber and increment it explicitly when we're done (iff we
    // ended with a '\n' or '\r' as opposed to EOF).
    //
    // Also, we need to undo the special casing done by BufferedReader.readLine
    // when a '\r' is the last char in the buffer.  That situation is marked
    // by 'pos > limit'.
    int tmpLineNumber = lineNumber;
    skipRedundantLF();
    String str = super.readLine();
    if (pos > limit)
      --pos;

    int ch;
    if (pos > 0 && ((ch = buffer[pos - 1]) == '\n' || ch == '\r'))
      lineNumber = tmpLineNumber + 1;

    return str;
  }

  public long skip(long count) throws IOException
  {
    if (count <= 0)
      return 0;
    long to_do = count;
    do
      {
	int ch = read();
	if (ch < 0)
	  break;
	to_do--;
	if (ch == '\n' || ch == '\r')
	  lineNumber++;
	else
	  {
	    long fence = pos + to_do;
	    if (limit < fence)
	      fence = limit;
	    int end = pos;
	    for (; end < fence; end++)
	      {
		char endch = buffer[end];
		if (endch == '\n' || endch == '\r')
		  break;
	      }
	    to_do -= end - pos;
	    pos = end;
	  }
      }
    while (to_do > 0);
    return count - to_do;
  }
}
