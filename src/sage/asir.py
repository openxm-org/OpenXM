# $OpenXM$
from __future__ import print_function
from __future__ import absolute_import

import os
from .expect import Expect, ExpectElement
from sage.misc.misc import verbose

class Asir(Expect):
    r"""
    Interface to the Asir interpreter.

    EXAMPLES::

        sage: asir.eval("a = [ 1, 1, 2; 3, 5, 8; 13, 21, 33 ]")    # optional - asir
        'a =\n\n 1 1 2\n 3 5 8\n 13 21 33\n\n's
        sage: asir.eval("b = [ 1; 3; 13]")                         # optional - asir
        'b =\n\n 1\n 3\n 13\n\n'
        sage: asir.eval("c=a \\ b") # solves linear equation: a*c = b  # optional - asir; random output
        'c =\n\n 1\n 7.21645e-16\n -7.21645e-16\n\n'
        sage: asir.eval("c")                                 # optional - asir; random output
        'c =\n\n 1\n 7.21645e-16\n -7.21645e-16\n\n'
    """

    def __init__(self, maxread=None, script_subdirectory=None, logfile=None,
            server=None, server_tmpdir=None, seed=None, command=None):
        """
        EXAMPLES::

            sage: asir == loads(dumps(asir))
            True
        """
        if command is None:
            import os
            command = os.getenv('SAGE_ASIR_COMMAND') or 'openxm ox_texmacs --view sage --quiet --noCopyright'
        if server is None:
            import os
            server = os.getenv('SAGE_ASIR_SERVER') or None
        Expect.__init__(self,
                        name = 'asir',
                        # We want the prompt sequence to be unique to avoid confusion with syntax error messages containing >>>
#                        prompt = 'asir\:\d+> ',
                        prompt = 'asir>',
#                        prompt = '',
                        # We don't want any pagination of output
#                        command = command + " --no-line-editing --silent --eval 'PS2(PS1());more off' --persist",
                        command = 'openxm ox_texmacs --view sage --quiet --noCopyright',
#                        command = "openxm asir -quiet",
                        maxread = maxread,
                        server = server,
                        server_tmpdir = server_tmpdir,
                        script_subdirectory = script_subdirectory,
                        restart_on_ctrlc = False,
                        verbose_start = False,
                        logfile = logfile,
                        eval_using_file_cutoff=100)
        self._seed = seed

    def set_seed(self, seed=None):
        """
        Sets the seed for the random number generator
        for this asir interpreter.

        EXAMPLES::

            sage: o = Asir() # optional - asir
            sage: o.set_seed(1) # optional - asir
            1
            sage: [o.rand() for i in range(5)] # optional - asir
            [ 0.134364,  0.847434,  0.763775,  0.255069,  0.495435]
        """
        if seed is None:
            seed = self.rand_seed()
        self.eval("rand('state',%d)" % seed)
        self._seed = seed
        return seed

    def __reduce__(self):
        """
        EXAMPLES::

            sage: asir.__reduce__()
            (<function reduce_load_Asir at 0x...>, ())
        """
        return reduce_load_Asir, tuple([])

    def _read_in_file_command(self, filename):
        """
        EXAMPLES::

            sage: filename = tmp_filename()
            sage: asir._read_in_file_command(filename)
            'source("...");'
        """
        return 'source("%s");'%filename

    def _quit_string(self):
        """
        EXAMPLES::

            sage: asir._quit_string()
            'quit;'
        """
        return '!quit;'

    def _install_hints(self):
        """
        Returns hints on how to install Asir.

        EXAMPLES::

            sage: print(asir._install_hints())
            You must get ...
        """
        return """
        You must get the program "asir" in order to use Asir
        from Sage.   You can read all about Asir at
                http://www.gnu.org/software/asir/

        LINUX:
           Do apt-get install asir as root on your machine (Ubuntu/Debian).
           Other Linux systems have asir too.

        OS X:
           * This website has details on OS X builds of Asir:
                    http://wiki.asir.org/Asir_for_MacOS_X
           * Darwin ports and fink have Asir as well.
        """

    def _eval_line(self, line, reformat=True, allow_use_file=False,
                   wait_for_prompt=True, restart_if_needed=False):
        """
        EXAMPLES::

            sage: print(asir._eval_line('2+2'))  #optional - asir
              ans =  4
        """
        from pexpect.exceptions import EOF
        if not wait_for_prompt:
            return Expect._eval_line(self, line)
        if line == '':
            return ''
        if self._expect is None:
            self._start()
        if allow_use_file and len(line)>3000:
            return self._eval_line_using_file(line)
        try:
            E = self._expect
            # debug
            # self._synchronize(cmd='1+%s\n')
            verbose("in = '%s'"%line,level=3)
            E.sendline(line)
            E.expect(self._prompt)
            out = E.before
            # debug
            verbose("out = '%s'"%out,level=3)
        except EOF:
            if self._quit_string() in line:
                return ''
        except KeyboardInterrupt:
            self._keyboard_interrupt()
        try:
            if reformat:
                if 'syntax error' in out:
                    raise SyntaxError(out)
            out = "\n".join(out.splitlines()[1:])
            return out
        except NameError:
            return ''

    def _keyboard_interrupt(self):
        print("CntrlC: Interrupting %s..."%self)
        if self._restart_on_ctrlc:
            try:
                self._expect.close(force=1)
            except pexpect.ExceptionPexpect as msg:
                raise RuntimeError( "THIS IS A BUG -- PLEASE REPORT. This should never happen.\n" + msg)
            self._start()
            raise KeyboardInterrupt("Restarting %s (WARNING: all variables defined in previous session are now invalid)"%self)
        else:
            self._expect.send('\003') # control-c
            raise KeyboardInterrupt("Ctrl-c pressed while running %s"%self)

    def quit(self, verbose=False):
        """
        EXAMPLES::

            sage: o = Asir()
            sage: o._start()    # optional - asir
            sage: o.quit(True)  # optional - asir
            Exiting spawned Asir process.
        """
        # Don't bother, since it just hangs in some cases, and it
        # isn't necessary, since asir behaves well with respect
        # to signals.
        if not self._expect is None:
            if verbose:
                print("Exiting spawned %s process." % self)
        return

    def _start(self):
        """
        Starts the Asir process.

        EXAMPLES::

            sage: o = Asir()    # optional - asir
            sage: o.is_running()  # optional - asir
            False
            sage: o._start()      # optional - asir
            sage: o.is_running()  # optional - asir
            True
        """
        Expect._start(self)
#        self.eval("page_screen_output=0;")
#        self.eval("format none;")
        # set random seed
#        self.set_seed(self._seed)

    def _equality_symbol(self):
        """
        EXAMPLES::

            sage: asir('0 == 1')  # optional - asir
             0
            sage: asir('1 == 1')  # optional - asir
             1
        """
        return '=='

    def _true_symbol(self):
        """
        EXAMPLES::

            sage: asir('1 == 1')  # optional - asir
             1
        """
        return '1'

    def _false_symbol(self):
        """
        EXAMPLES::

            sage: asir('0 == 1')  # optional - asir
             0
        """
        return '0'

    def set(self, var, value):
        """
        Set the variable ``var`` to the given ``value``.

        EXAMPLES::

            sage: asir.set('x', '2') # optional - asir
            sage: asir.get('x') # optional - asir
            ' 2'
        """
        cmd = '%s=%s;'%(var,value)
        out = self.eval(cmd)
        if out.find("error") != -1 or out.find("Error") != -1:
            raise TypeError("Error executing code in Asir\nCODE:\n\t%s\nAsir ERROR:\n\t%s"%(cmd, out))

    def get(self, var):
        """
        Get the value of the variable ``var``.

        EXAMPLES::

            sage: asir.set('x', '2') # optional - asir
            sage: asir.get('x') # optional - asir
            ' 2'
        """
        s = self.eval('%s;'%var)
        i = s.find('=')
        return s[i+1:]

    def clear(self, var):
        """
        Clear the variable named var.

        EXAMPLES::

            sage: asir.set('x', '2') # optional - asir
            sage: asir.clear('x')    # optional - asir
            sage: asir.get('x')      # optional - asir
            "error: 'x' undefined near line ... column 1"
        """
        self.eval('clear %s'%var)

    def console(self):
        """
        Spawn a new Asir command-line session.

        This requires that the optional asir program be installed and in
        your PATH, but no optional Sage packages need be installed.

        EXAMPLES::

            sage: asir_console()         # not tested
            GNU Asir, version 2.1.73 (i386-apple-darwin8.5.3).
            Copyright (C) 2006 John W. Eaton.
            ...
            asir:1> 2+3
            ans = 5
            asir:2> [ctl-d]

        Pressing ctrl-d exits the asir console and returns you to Sage.
        asir, like Sage, remembers its history from one session to
        another.
        """
        asir_console()

    def version(self):
        """
        Return the version of Asir.

        OUTPUT: string

        EXAMPLES::

            sage: v = asir.version()   # optional - asir
            sage: v                      # optional - asir; random
            '2.13.7'

            sage: import re
            sage: assert re.match("\d+\.\d+\.\d+", v)  is not None # optional - asir
        """
        return str(self("version()")).strip()

    def solve_linear_system(self, A, b):
        r"""
        Use asir to compute a solution x to A\*x = b, as a list.

        INPUT:

        - ``A`` -- mxn matrix A with entries in `\QQ` or `\RR`

        - ``b`` -- m-vector b entries in `\QQ` or `\RR` (resp)

        OUTPUT: A list x (if it exists) which solves M\*x = b

        EXAMPLES::

            sage: M33 = MatrixSpace(QQ,3,3)
            sage: A   = M33([1,2,3,4,5,6,7,8,0])
            sage: V3  = VectorSpace(QQ,3)
            sage: b   = V3([1,2,3])
            sage: asir.solve_linear_system(A,b)    # optional - asir (and output is slightly random in low order bits)
            [-0.33333299999999999, 0.66666700000000001, -3.5236600000000002e-18]

        AUTHORS:

        - David Joyner and William Stein
        """
        m = A.nrows()
        if m != len(b):
            raise ValueError("dimensions of A and b must be compatible")
        from sage.matrix.all import MatrixSpace
        from sage.rings.all import QQ
        MS = MatrixSpace(QQ,m,1)
        b  = MS(list(b)) # converted b to a "column vector"
        sA = self.sage2asir_matrix_string(A)
        sb = self.sage2asir_matrix_string(b)
        self.eval("a = " + sA )
        self.eval("b = " + sb )
        soln = asir.eval("c = a \\ b")
        soln = soln.replace("\n\n ","[")
        soln = soln.replace("\n\n","]")
        soln = soln.replace("\n",",")
        sol  = soln[3:]
        return eval(sol)


    def sage2asir_matrix_string(self, A):
        """
        Return an asir matrix from a Sage matrix.

        INPUT: A Sage matrix with entries in the rationals or reals.

        OUTPUT: A string that evaluates to an Asir matrix.

        EXAMPLES::

            sage: M33 = MatrixSpace(QQ,3,3)
            sage: A = M33([1,2,3,4,5,6,7,8,0])
            sage: asir.sage2asir_matrix_string(A)   # optional - asir
            '[1, 2, 3; 4, 5, 6; 7, 8, 0]'

        AUTHORS:

        - David Joyner and William Stein
        """
        return str(A.rows()).replace('), (', '; ').replace('(', '').replace(')','')

    def de_system_plot(self, f, ics, trange):
        r"""
        Plots (using asir's interface to gnuplot) the solution to a
        `2\times 2` system of differential equations.

        INPUT:


        -  ``f`` - a pair of strings representing the
           differential equations; The independent variable must be called x
           and the dependent variable must be called y.

        -  ``ics`` - a pair [x0,y0] such that x(t0) = x0, y(t0)
           = y0

        -  ``trange`` - a pair [t0,t1]


        OUTPUT: a gnuplot window appears

        EXAMPLES::

            sage: asir.de_system_plot(['x+y','x-y'], [1,-1], [0,2])  # not tested -- does this actually work (on OS X it fails for me -- William Stein, 2007-10)

        This should yield the two plots `(t,x(t)), (t,y(t))` on the
        same graph (the `t`-axis is the horizontal axis) of the
        system of ODEs

        .. math::

                       x' = x+y, x(0) = 1;\qquad y' = x-y, y(0) = -1,                     \quad\text{for}\quad 0 < t < 2.
        """
        eqn1 = f[0].replace('x','x(1)').replace('y','x(2)')
        eqn2 = f[1].replace('x','x(1)').replace('y','x(2)')
        fcn = "function xdot = f(x,t) xdot(1) = %s; xdot(2) = %s; endfunction"%(eqn1, eqn2)
        self.eval(fcn)
        x0_eqn = "x0 = [%s; %s]"%(ics[0], ics[1])
        self.eval(x0_eqn)
        t_eqn = "t = linspace(%s, %s, 200)'"%(trange[0], trange[1])
        self.eval(t_eqn)
        x_eqn = 'x = lsode("f",x0,t);'
        self.eval(x_eqn)
        self.eval("plot(t,x)")

    def _object_class(self):
        """
        EXAMPLES::

            sage: asir._object_class()
            <class 'sage.interfaces.asir.AsirElement'>
        """
        return AsirElement


asir_functions = set()

def to_complex(asir_string, R):
    r"""
    Helper function to convert asir complex number

    TESTS::

        sage: from sage.interfaces.asir import to_complex
        sage: to_complex('(0,1)', CDF)
        1.0*I
        sage: to_complex('(1.3231,-0.2)', CDF)
        1.3231 - 0.2*I
    """
    real, imag = asir_string.strip('() ').split(',')
    return R(float(real), float(imag))

class AsirElement(ExpectElement):
    def _get_sage_ring(self):
        r"""
        TESTS::

            sage: asir('1')._get_sage_ring()  # optional - asir
            Real Double Field
            sage: asir('I')._get_sage_ring()  # optional - asir
            Complex Double Field
            sage: asir('[]')._get_sage_ring() # optional - asir
            Real Double Field
        """
        if self.isinteger():
            import sage.rings.integer_ring
            return sage.rings.integer_ring.ZZ
        elif self.isreal():
            import sage.rings.real_double
            return sage.rings.real_double.RDF
        elif self.iscomplex():
            import sage.rings.complex_double
            return sage.rings.complex_double.CDF
        else:
            raise TypeError("no Sage ring associated to this element.")

    def __nonzero__(self):
        r"""
        Test whether this element is nonzero.

        EXAMPLES::

            sage: bool(asir('0'))                 # optional - asir
            False
            sage: bool(asir('[]'))                # optional - asir
            False
            sage: bool(asir('[0,0]'))             # optional - asir
            False
            sage: bool(asir('[0,0,0;0,0,0]'))     # optional - asir
            False

            sage: bool(asir('0.1'))               # optional - asir
            True
            sage: bool(asir('[0,1,0]'))           # optional - asir
            True
            sage: bool(asir('[0,0,-0.1;0,0,0]'))  # optional - asir
            True
        """
        return str(self) != ' [](0x0)' and any(x != '0' for x in str(self).split())

    def _matrix_(self, R=None):
        r"""
        Return Sage matrix from this asir element.

        EXAMPLES::

            sage: A = asir('[1,2;3,4.5]')     # optional - asir
            sage: matrix(A)                     # optional - asir
            [1.0 2.0]
            [3.0 4.5]
            sage: _.base_ring()                 # optional - asir
            Real Double Field

            sage: A = asir('[I,1;-1,0]')      # optional - asir
            sage: matrix(A)                     # optional - asir
            [1.0*I   1.0]
            [ -1.0   0.0]
            sage: _.base_ring()                 # optional - asir
            Complex Double Field

            sage: A = asir('[1,2;3,4]')       # optional - asir
            sage: matrix(ZZ, A)                 # optional - asir
            [1 2]
            [3 4]
            sage: A = asir('[1,2;3,4.5]')     # optional - asir
            sage: matrix(RR, A)                 # optional - asir
            [1.00000000000000 2.00000000000000]
            [3.00000000000000 4.50000000000000]
        """
        if not self.ismatrix():
            raise TypeError('not an asir matrix')
        if R is None:
            R = self._get_sage_ring()

        s = str(self).strip('\n ')
        w = [u.strip().split(' ') for u in s.split('\n')]
        nrows = len(w)
        ncols = len(w[0])

        if self.iscomplex():
            w = [[to_complex(x,R) for x in row] for row in w]

        from sage.matrix.all import MatrixSpace
        return MatrixSpace(R, nrows, ncols)(w)

    def _vector_(self, R=None):
        r"""
        Return Sage vector from this asir element.

        EXAMPLES::

            sage: A = asir('[1,2,3,4]')       # optional - asir
            sage: vector(ZZ, A)                 # optional - asir
            (1, 2, 3, 4)
            sage: A = asir('[1,2.3,4.5]')     # optional - asir
            sage: vector(A)                     # optional - asir
            (1.0, 2.3, 4.5)
            sage: A = asir('[1,I]')           # optional - asir
            sage: vector(A)                     # optional - asir
            (1.0, 1.0*I)
        """
        oc = self.parent()
        if not self.isvector():
            raise TypeError('not an asir vector')
        if R is None:
            R = self._get_sage_ring()

        s = str(self).strip('\n ')
        w = s.strip().split(' ')
        nrows = len(w)

        if self.iscomplex():
            w = [to_complex(x, R) for x in w]

        from sage.modules.free_module import FreeModule
        return FreeModule(R, nrows)(w)

    def _scalar_(self):
        """
        Return Sage scalar from this asir element.

        EXAMPLES::

            sage: A = asir('2833')      # optional - asir
            sage: As = A.sage(); As       # optional - asir
            2833.0
            sage: As.parent()             # optional - asir
            Real Double Field

            sage: B = sqrt(A)             # optional - asir
            sage: Bs = B.sage(); Bs       # optional - asir
            53.2259
            sage: Bs.parent()             # optional - asir
            Real Double Field

            sage: C = sqrt(-A)            # optional - asir
            sage: Cs = C.sage(); Cs       # optional - asir
            53.2259*I
            sage: Cs.parent()             # optional - asir
            Complex Double Field
        """
        if not self.isscalar():
            raise TypeError("not an asir scalar")

        R = self._get_sage_ring()
        if self.iscomplex():
            return to_complex(str(self), R)
        else:
            return R(str(self))

    def _sage_(self):
        """
        Try to parse the asir object and return a sage object.

        EXAMPLES::

            sage: A = asir('2833')           # optional - asir
            sage: A.sage()                     # optional - asir
            2833.0
            sage: B = sqrt(A)                  # optional - asir
            sage: B.sage()                     # optional - asir
            53.2259
            sage: C = sqrt(-A)                 # optional - asir
            sage: C.sage()                     # optional - asir
            53.2259*I
            sage: A = asir('[1,2,3,4]')      # optional - asir
            sage: A.sage()                     # optional - asir
            (1.0, 2.0, 3.0, 4.0)
            sage: A = asir('[1,2.3,4.5]')    # optional - asir
            sage: A.sage()                     # optional - asir
            (1.0, 2.3, 4.5)
            sage: A = asir('[1,2.3+I,4.5]')  # optional - asir
            sage: A.sage()                     # optional - asir
            (1.0, 2.3 + 1.0*I, 4.5)
        """
        if self.isscalar():
            return self._scalar_()
        elif self.isvector():
            return self._vector_()
        elif self.ismatrix():
            return self._matrix_()
        else:
            raise NotImplementedError('asir type is not recognized')

# An instance
asir = Asir()

def reduce_load_Asir():
    """
    EXAMPLES::

        sage: from sage.interfaces.asir import reduce_load_Asir
        sage: reduce_load_Asir()
        Asir
    """
    return asir


def asir_console():
    """
    Spawn a new Asir command-line session.

    This requires that the optional asir program be installed and in
    your PATH, but no optional Sage packages need be installed.

    EXAMPLES::

        sage: asir_console()         # not tested
        GNU Asir, version 2.1.73 (i386-apple-darwin8.5.3).
        Copyright (C) 2006 John W. Eaton.
        ...
        asir:1> 2+3
        ans = 5
        asir:2> [ctl-d]

    Pressing ctrl-d exits the asir console and returns you to Sage.
    asir, like Sage, remembers its history from one session to
    another.
    """
    from sage.repl.rich_output.display_manager import get_display_manager
    if not get_display_manager().is_in_terminal():
        raise RuntimeError('Can use the console only in the terminal. Try %%asir magics instead.')
    os.system('openxm asir')    # with asir prompt
#    os.system('openxm asir -quiet')


def asir_version():
    """
    DEPRECATED: Return the version of Asir installed.

    EXAMPLES::

        sage: asir_version()    # optional - asir
        doctest:...: DeprecationWarning: This has been deprecated. Use
        asir.version() instead
        See http://trac.sagemath.org/21135 for details.
        '...'
    """
    from sage.misc.superseded import deprecation
    deprecation(21135, "This has been deprecated. Use asir.version() instead")
    return asir.version()
