/* ------------------------------------------------------------
author: "Michel Buffa & Jerome Lebrun"
name: "guitar tube amp sim 100% FAUST"
version: "0.1"
Code generated with Faust 2.83.1 (https://faust.grame.fr)
Compilation options: -a /opt/homebrew/Cellar/faust/2.83.1_1/share/faust/sndfile.cpp -lang cpp -i -fpga-mem-th 4 -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

/************************************************************************
 IMPORTANT NOTE : this file contains two clearly delimited sections :
 the ARCHITECTURE section (in two parts) and the USER section. Each section
 is governed by its own copyright and license. Please check individually
 each section for license and copyright information.
 *************************************************************************/

/******************* BEGIN sndfile.cpp ****************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2021 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 
 ************************************************************************
 ************************************************************************/

#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sndfile.h>
#include <vector>
#include <stack>
#include <string>
#include <map>
#include <iostream>

/************************** BEGIN dsp.h ********************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>
#include <vector>
#include <cstdint>

/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ***************************************************************************/

#ifndef __export__
#define __export__

// Version as a global string
#define FAUSTVERSION "2.83.1"

// Version as separated [major,minor,patch] values
#define FAUSTMAJORVERSION 2
#define FAUSTMINORVERSION 83
#define FAUSTPATCHVERSION 1

// Use FAUST_API for code that is part of the external API but is also compiled in faust and libfaust
// Use LIBFAUST_API for code that is compiled in faust and libfaust

#ifdef _WIN32
    #pragma warning (disable: 4251)
    #ifdef FAUST_EXE
        #define FAUST_API
        #define LIBFAUST_API
    #elif FAUST_LIB
        #define FAUST_API __declspec(dllexport)
        #define LIBFAUST_API __declspec(dllexport)
    #else
        #define FAUST_API
        #define LIBFAUST_API 
    #endif
#else
    #ifdef FAUST_EXE
        #define FAUST_API
        #define LIBFAUST_API
    #else
        #define FAUST_API __attribute__((visibility("default")))
        #define LIBFAUST_API __attribute__((visibility("default")))
    #endif
#endif

#endif

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

struct FAUST_API UI;
struct FAUST_API Meta;

/**
 * DSP memory manager.
 */

struct FAUST_API dsp_memory_manager {
    
    enum MemType { kInt32, kInt32_ptr, kFloat, kFloat_ptr, kDouble, kDouble_ptr, kQuad, kQuad_ptr, kFixedPoint, kFixedPoint_ptr, kObj, kObj_ptr, kSound, kSound_ptr };

    virtual ~dsp_memory_manager() = default;
    
    /**
     * Inform the Memory Manager with the number of expected memory zones.
     * @param count - the number of expected memory zones
     */
    virtual void begin(size_t count) {}
    
    /**
     * Give the Memory Manager information on a given memory zone.
     * @param name - the memory zone name
     * @param type - the memory zone type (in MemType)
     * @param size - the size in unit of the memory type of the memory zone
     * @param size_bytes - the size in bytes of the memory zone
     * @param reads - the number of Read access to the zone used to compute one frame
     * @param writes - the number of Write access to the zone used to compute one frame
     */
    virtual void info(const char* name, MemType type, size_t size, size_t size_bytes, size_t reads, size_t writes) {}
  
    /**
     * Inform the Memory Manager that all memory zones have been described,
     * to possibly start a 'compute the best allocation strategy' step.
     */
    virtual void end() {}
    
    /**
     * Allocate a memory zone.
     * @param size - the memory zone size in bytes
     */
    virtual void* allocate(size_t size) = 0;
    
    /**
     * Destroy a memory zone.
     * @param ptr - the memory zone pointer to be deallocated
     */
    virtual void destroy(void* ptr) = 0;
    
};

/**
* Signal processor definition.
*/

class FAUST_API dsp {

    public:

        dsp() = default;
        virtual ~dsp() = default;

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the ui_interface parameter with instance specific calls
         * to 'openTabBox', 'addButton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Return the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /**
         * Global init, calls the following methods:
         * - static class 'classInit': static tables initialization
         * - 'instanceInit': constants and instance state initialization
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void init(int sample_rate) = 0;

        /**
         * Init instance state.
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceInit(int sample_rate) = 0;
    
        /**
         * Init instance constant state.
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceConstants(int sample_rate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (like delay lines...) but keep the control parameter values */
        virtual void instanceClear() = 0;
 
        /**
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual ::dsp* clone() = 0;
    
        /**
         * Trigger the Meta* m parameter with instance specific calls to 'declare' (key, value) metadata.
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * Read all controllers (buttons, sliders, etc.), and update the DSP state to be used by 'frame' or 'compute'.
         * This method will be filled with the -ec (--external-control) option.
         */
        virtual void control() {}
    
        /**
         * DSP instance computation to process one single frame.
         *
         * Note that by default inputs and outputs buffers are supposed to be distinct memory zones,
         * so one cannot safely write frame(inputs, inputs).
         * The -inpl option can be used for that, but only in scalar mode for now.
         * This method will be filled with the -os (--one-sample) option.
         *
         * @param inputs - the input audio buffers as an array of FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of FAUSTFLOAT samples (eiher float, double or quad)
         */
        virtual void frame(FAUSTFLOAT* inputs, FAUSTFLOAT* outputs) {}
        
        /**
         * DSP instance computation to be called with successive in/out audio buffers.
         *
         * Note that by default inputs and outputs buffers are supposed to be distinct memory zones,
         * so one cannot safely write compute(count, inputs, inputs).
         * The -inpl compilation option can be used for that, but only in scalar mode for now.
         *
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT buffers
         * (containing either float, double or quad samples)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT buffers
         * (containing either float, double or quad samples)
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * Alternative DSP instance computation method for use by subclasses, incorporating an additional `date_usec` parameter,
         * which specifies the timestamp of the first sample in the audio buffers.
         *
         * @param date_usec - the timestamp in microsec given by audio driver. By convention timestamp of -1 means 'no timestamp conversion',
         * events already have a timestamp expressed in frames.
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         */
        virtual void compute(double /*date_usec*/, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

/**
 * Generic DSP decorator.
 */

class FAUST_API decorator_dsp : public ::dsp {

    protected:

        ::dsp* fDSP;

    public:

        decorator_dsp(::dsp* dsp = nullptr):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() override { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() override { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) override { fDSP->buildUserInterface(ui_interface); }
        virtual int getSampleRate() override { return fDSP->getSampleRate(); }
        virtual void init(int sample_rate) override { fDSP->init(sample_rate); }
        virtual void instanceInit(int sample_rate) override { fDSP->instanceInit(sample_rate); }
        virtual void instanceConstants(int sample_rate) override { fDSP->instanceConstants(sample_rate); }
        virtual void instanceResetUserInterface() override { fDSP->instanceResetUserInterface(); }
        virtual void instanceClear() override { fDSP->instanceClear(); }
        virtual decorator_dsp* clone() override { return new decorator_dsp(fDSP->clone()); }
        virtual void metadata(Meta* m) override { fDSP->metadata(m); }
        // Beware: subclasses usually have to overload the two 'compute' methods
        virtual void control() override { fDSP->control(); }
        virtual void frame(FAUSTFLOAT* inputs, FAUSTFLOAT* outputs) override { fDSP->frame(inputs, outputs); }
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) override { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) override { fDSP->compute(date_usec, count, inputs, outputs); }
    
};

/**
 * DSP factory class, used with LLVM and Interpreter backends
 * to create DSP instances from a compiled DSP program.
 */

class FAUST_API dsp_factory {
    
    protected:
    
        // So that to force sub-classes to use deleteDSPFactory(dsp_factory* factory);
        virtual ~dsp_factory() = default;
    
    public:
    
        /* Return factory name */
        virtual std::string getName() = 0;
    
        /* Return factory SHA key */
        virtual std::string getSHAKey() = 0;
    
        /* Return factory expanded DSP code */
        virtual std::string getDSPCode() = 0;
    
        /* Return factory compile options */
        virtual std::string getCompileOptions() = 0;
    
        /* Get the Faust DSP factory list of library dependancies */
        virtual std::vector<std::string> getLibraryList() = 0;
    
        /* Get the list of all used includes */
        virtual std::vector<std::string> getIncludePathnames() = 0;
    
        /* Get warning messages list for a given compilation */
        virtual std::vector<std::string> getWarningMessages() = 0;
    
        /* Create a new DSP instance, to be deleted with C++ 'delete' */
        virtual ::dsp* createDSPInstance() = 0;
    
        /* Static tables initialization, possibly implemened in sub-classes*/
        virtual void classInit(int sample_rate) {};
    
        /* Set a custom memory manager to be used when creating instances */
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
    
        /* Return the currently set custom memory manager */
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
};

// Denormal handling

#if defined (__SSE__)
#include <xmmintrin.h>
#endif

class FAUST_API ScopedNoDenormals {
    
    private:
    
        intptr_t fpsr = 0;
        
        void setFpStatusRegister(intptr_t fpsr_aux) noexcept
        {
        #if defined (__arm64__) || defined (__aarch64__)
            asm volatile("msr fpcr, %0" : : "ri" (fpsr_aux));
        #elif defined (__SSE__)
            // The volatile keyword here is needed to workaround a bug in AppleClang 13.0
            // which aggressively optimises away the variable otherwise
            volatile uint32_t fpsr_w = static_cast<uint32_t>(fpsr_aux);
            _mm_setcsr(fpsr_w);
        #endif
        }
        
        void getFpStatusRegister() noexcept
        {
        #if defined (__arm64__) || defined (__aarch64__)
            asm volatile("mrs %0, fpcr" : "=r" (fpsr));
        #elif defined (__SSE__)
            fpsr = static_cast<intptr_t>(_mm_getcsr());
        #endif
        }
    
    public:
    
        ScopedNoDenormals() noexcept
        {
        #if defined (__arm64__) || defined (__aarch64__)
            intptr_t mask = (1 << 24 /* FZ */);
        #elif defined (__SSE__)
        #if defined (__SSE2__)
            intptr_t mask = 0x8040;
        #else
            intptr_t mask = 0x8000;
        #endif
        #else
            intptr_t mask = 0x0000;
        #endif
            getFpStatusRegister();
            setFpStatusRegister(fpsr | mask);
        }
        
        ~ScopedNoDenormals() noexcept
        {
            setFpStatusRegister(fpsr);
        }

};

#define AVOIDDENORMALS ScopedNoDenormals ftz_scope;

#endif

/************************** END dsp.h **************************/
/************************** BEGIN console.h **************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 *************************************************************************/


#ifndef __faustconsole__
#define __faustconsole__

#include <stack>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <atomic>
#include <stdio.h>

/************************** BEGIN GUI.h **********************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 *************************************************************************/

#ifndef __GUI_H__
#define __GUI_H__

#include <list>
#include <map>
#include <vector>
#include <assert.h>

#ifdef _WIN32
# pragma warning (disable: 4100)
#else
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

/************************** BEGIN UI.h *****************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ********************************************************************/

#ifndef __UI_H__
#define __UI_H__


#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

struct Soundfile;

template <typename REAL>
struct FAUST_API UIReal {
    
    UIReal() {}
    virtual ~UIReal() {}
    
    // -- widget's layouts
    
    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;
    
    // -- active widgets
    
    virtual void addButton(const char* label, REAL* zone) = 0;
    virtual void addCheckButton(const char* label, REAL* zone) = 0;
    virtual void addVerticalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addHorizontalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addNumEntry(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    
    // -- passive widgets
    
    virtual void addHorizontalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    virtual void addVerticalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    
    // -- soundfiles
    
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) = 0;
    
    // -- metadata declarations
    
    virtual void declare(REAL* /*zone*/, const char* /*key*/, const char* /*val*/) {}

    // To be used by LLVM client
    virtual int sizeOfFAUSTFLOAT() { return sizeof(FAUSTFLOAT); }
};

struct FAUST_API UI : public UIReal<FAUSTFLOAT> {
    UI() {}
    virtual ~UI() {}
#ifdef DAISY_NO_RTTI
    virtual bool isSoundUI() const { return false; }
    virtual bool isMidiInterface() const { return false; }
#endif
};

#endif
/**************************  END  UI.h **************************/
/************************** BEGIN ValueConverter.h ********************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ********************************************************************/

#ifndef __ValueConverter__
#define __ValueConverter__

/***************************************************************************************
 ValueConverter.h
 (GRAME, Copyright 2015-2019)
 
 Set of conversion objects used to map user interface values (for example a gui slider
 delivering values between 0 and 1) to Faust values (for example a vslider between
 20 and 20000) using a log scale.
 
 -- Utilities
 
 Range(lo,hi) : clip a value x between lo and hi
 Interpolator(lo,hi,v1,v2) : Maps a value x between lo and hi to a value y between v1 and v2
 Interpolator3pt(lo,mi,hi,v1,vm,v2) : Map values between lo mid hi to values between v1 vm v2
 
 -- Value Converters
 
 ValueConverter::ui2faust(x)
 ValueConverter::faust2ui(x)
 
 -- ValueConverters used for sliders depending of the scale
 
 LinearValueConverter(umin, umax, fmin, fmax)
 LinearValueConverter2(lo, mi, hi, v1, vm, v2) using 2 segments
 LogValueConverter(umin, umax, fmin, fmax)
 ExpValueConverter(umin, umax, fmin, fmax)
 
 -- ValueConverters used for accelerometers based on 3 points
 
 UpConverter(amin, amid, amax, fmin, fmid, fmax)        -- curve 0
 DownConverter(amin, amid, amax, fmin, fmid, fmax)      -- curve 1
 UpDownConverter(amin, amid, amax, fmin, fmid, fmax)    -- curve 2
 DownUpConverter(amin, amid, amax, fmin, fmid, fmax)    -- curve 3
 
 -- lists of ZoneControl are used to implement accelerometers metadata for each axes
 
 ZoneControl(zone, valueConverter) : a zone with an accelerometer data converter
 
 -- ZoneReader are used to implement screencolor metadata
 
 ZoneReader(zone, valueConverter) : a zone with a data converter

****************************************************************************************/

#include <float.h>
#include <algorithm>    // std::max
#include <cmath>
#include <vector>
#include <assert.h>


//--------------------------------------------------------------------------------------
// Interpolator(lo,hi,v1,v2)
// Maps a value x between lo and hi to a value y between v1 and v2
// y = v1 + (x-lo)/(hi-lo)*(v2-v1)
// y = v1 + (x-lo) * coef           with coef = (v2-v1)/(hi-lo)
// y = v1 + x*coef - lo*coef
// y = v1 - lo*coef + x*coef
// y = offset + x*coef              with offset = v1 - lo*coef
//--------------------------------------------------------------------------------------
class FAUST_API Interpolator {
    
    private:

        //--------------------------------------------------------------------------------------
        // Range(lo,hi) clip a value between lo and hi
        //--------------------------------------------------------------------------------------
        struct Range
        {
            double fLo;
            double fHi;

            Range(double x, double y) : fLo(std::min<double>(x,y)), fHi(std::max<double>(x,y)) {}
            double operator()(double x) const noexcept { return (x<fLo) ? fLo : (x>fHi) ? fHi : x; }
        };

        Range fRange;
        double fCoef;
        double fOffset;

    public:

        Interpolator(double lo, double hi, double v1, double v2) : fRange(lo,hi)
        {
            if (hi != lo) {
                // regular case
                fCoef = (v2-v1)/(hi-lo);
                fOffset = v1 - lo*fCoef;
            } else {
                // degenerate case, avoids division by zero
                fCoef = 0;
                fOffset = (v1+v2)/2;
            }
        }
        double operator()(double v) const noexcept
        {
            double x = fRange(v);
            return  fOffset + x*fCoef;
        }

        double coef() const noexcept { return fCoef; }

        void getLowHigh(double& amin, double& amax) const noexcept
        {
            amin = fRange.fLo;
            amax = fRange.fHi;
        }
};

//--------------------------------------------------------------------------------------
// Interpolator3pt(lo,mi,hi,v1,vm,v2)
// Map values between lo mid hi to values between v1 vm v2
//--------------------------------------------------------------------------------------
class FAUST_API Interpolator3pt {

    private:

        Interpolator fSegment1;
        Interpolator fSegment2;
        double fMid;

    public:

        Interpolator3pt(double lo, double mi, double hi, double v1, double vm, double v2) :
            fSegment1(lo, mi, v1, vm),
            fSegment2(mi, hi, vm, v2),
            fMid(mi) {}
        double operator()(double x) const noexcept { return  (x < fMid) ? fSegment1(x) : fSegment2(x); }

        void getMappingValues(double& amin, double& amid, double& amax) const noexcept
        {
            fSegment1.getLowHigh(amin, amid);
            fSegment2.getLowHigh(amid, amax);
        }
};

//--------------------------------------------------------------------------------------
// Abstract ValueConverter class. Converts values between UI and Faust representations
//--------------------------------------------------------------------------------------
class FAUST_API ValueConverter {

    public:

        virtual ~ValueConverter() {}
        virtual double ui2faust(double x) { return x; };
        virtual double faust2ui(double x) { return x; };
};

//--------------------------------------------------------------------------------------
// A converter than can be updated
//--------------------------------------------------------------------------------------

class FAUST_API UpdatableValueConverter : public ValueConverter {
    
    protected:
        
        bool fActive;
        
    public:
        
        UpdatableValueConverter():fActive(true)
        {}
        virtual ~UpdatableValueConverter()
        {}
        
        virtual void setMappingValues(double amin, double amid, double amax, double min, double init, double max) = 0;
        virtual void getMappingValues(double& amin, double& amid, double& amax) = 0;
        
        void setActive(bool on_off) { fActive = on_off; }
        bool getActive() { return fActive; }
    
};

//--------------------------------------------------------------------------------------
// Linear conversion between ui and Faust values
//--------------------------------------------------------------------------------------
class FAUST_API LinearValueConverter : public ValueConverter {
    
    private:
        
        Interpolator fUI2F;
        Interpolator fF2UI;
        
    public:
        
        LinearValueConverter(double umin, double umax, double fmin, double fmax) :
            fUI2F(umin,umax,fmin,fmax), fF2UI(fmin,fmax,umin,umax)
        {}
        
        LinearValueConverter() : fUI2F(0.,0.,0.,0.), fF2UI(0.,0.,0.,0.)
        {}
        virtual double ui2faust(double x) { return fUI2F(x); }
        virtual double faust2ui(double x)
        {
            // Avoid division by zero; if coef is zero, fall back to midpoint in the clamped range.
            if (fF2UI.coef() != 0.0) {
                return fF2UI(x);
            } else {
                double lo, hi;
                fF2UI.getLowHigh(lo, hi);
                double mid = (lo + hi) / 2.0;
                return fF2UI(mid);
            }
        }
    
};

//--------------------------------------------------------------------------------------
// Two segments linear conversion between ui and Faust values
//--------------------------------------------------------------------------------------
class FAUST_API LinearValueConverter2 : public UpdatableValueConverter {
    
    private:
    
        Interpolator3pt fUI2F;
        Interpolator3pt fF2UI;
        
    public:
    
        LinearValueConverter2(double amin, double amid, double amax, double min, double init, double max) :
            fUI2F(amin, amid, amax, min, init, max), fF2UI(min, init, max, amin, amid, amax)
        {}
        
        LinearValueConverter2() : fUI2F(0.,0.,0.,0.,0.,0.), fF2UI(0.,0.,0.,0.,0.,0.)
        {}
    
        virtual double ui2faust(double x) { return fUI2F(x); }
        virtual double faust2ui(double x) { return fF2UI(x); }
    
        virtual void setMappingValues(double amin, double amid, double amax, double min, double init, double max)
        {
            fUI2F = Interpolator3pt(amin, amid, amax, min, init, max);
            fF2UI = Interpolator3pt(min, init, max, amin, amid, amax);
        }

        virtual void getMappingValues(double& amin, double& amid, double& amax)
        {
            fUI2F.getMappingValues(amin, amid, amax);
        }
    
};

//--------------------------------------------------------------------------------------
// Logarithmic conversion between ui and Faust values
//--------------------------------------------------------------------------------------
class FAUST_API LogValueConverter : public LinearValueConverter {

    public:
    
        // We use DBL_EPSILON which is bigger than DBL_MIN (safer)
        LogValueConverter(double umin, double umax, double fmin, double fmax) :
            LinearValueConverter(umin, umax, std::log(std::max<double>(DBL_EPSILON, fmin)), std::log(std::max<double>(DBL_EPSILON, fmax)))
        {}

        virtual double ui2faust(double x) { return std::exp(LinearValueConverter::ui2faust(x)); }
        virtual double faust2ui(double x) { return LinearValueConverter::faust2ui(std::log(std::max<double>(DBL_EPSILON, x))); }

};

//--------------------------------------------------------------------------------------
// Exponential conversion between ui and Faust values
//--------------------------------------------------------------------------------------
class FAUST_API ExpValueConverter : public LinearValueConverter {

    public:

        ExpValueConverter(double umin, double umax, double fmin, double fmax) :
            LinearValueConverter(umin, umax, std::min<double>(DBL_MAX, std::exp(fmin)), std::min<double>(DBL_MAX, std::exp(fmax)))
        {}

        virtual double ui2faust(double x) { return std::log(LinearValueConverter::ui2faust(x)); }
        virtual double faust2ui(double x) { return LinearValueConverter::faust2ui(std::min<double>(DBL_MAX, std::exp(x))); }

};

//--------------------------------------------------------------------------------------
// Convert accelerometer or gyroscope values to Faust values
// Using an Up curve (curve 0)
//--------------------------------------------------------------------------------------
class FAUST_API UpConverter : public UpdatableValueConverter {

    private:

        Interpolator3pt fA2F;
        Interpolator3pt fF2A;

    public:

        UpConverter(double amin, double amid, double amax, double fmin, double fmid, double fmax) :
            fA2F(amin,amid,amax,fmin,fmid,fmax),
            fF2A(fmin,fmid,fmax,amin,amid,amax)
        {}

        virtual double ui2faust(double x) { return fA2F(x); }
        virtual double faust2ui(double x) { return fF2A(x); }

        virtual void setMappingValues(double amin, double amid, double amax, double fmin, double fmid, double fmax)
        {
            //__android_log_print(ANDROID_LOG_ERROR, "Faust", "UpConverter update %f %f %f %f %f %f", amin,amid,amax,fmin,fmid,fmax);
            fA2F = Interpolator3pt(amin, amid, amax, fmin, fmid, fmax);
            fF2A = Interpolator3pt(fmin, fmid, fmax, amin, amid, amax);
        }

        virtual void getMappingValues(double& amin, double& amid, double& amax)
        {
            fA2F.getMappingValues(amin, amid, amax);
        }

};

//--------------------------------------------------------------------------------------
// Convert accelerometer or gyroscope values to Faust values
// Using a Down curve (curve 1)
//--------------------------------------------------------------------------------------
class FAUST_API DownConverter : public UpdatableValueConverter {

    private:

        Interpolator3pt	fA2F;
        Interpolator3pt	fF2A;

    public:

        DownConverter(double amin, double amid, double amax, double fmin, double fmid, double fmax) :
            fA2F(amin,amid,amax,fmax,fmid,fmin),
            fF2A(fmin,fmid,fmax,amax,amid,amin)
        {}

        virtual double ui2faust(double x) { return fA2F(x); }
        virtual double faust2ui(double x) { return fF2A(x); }

        virtual void setMappingValues(double amin, double amid, double amax, double fmin, double fmid, double fmax)
        {
             //__android_log_print(ANDROID_LOG_ERROR, "Faust", "DownConverter update %f %f %f %f %f %f", amin,amid,amax,fmin,fmid,fmax);
            fA2F = Interpolator3pt(amin, amid, amax, fmax, fmid, fmin);
            fF2A = Interpolator3pt(fmin, fmid, fmax, amax, amid, amin);
        }

        virtual void getMappingValues(double& amin, double& amid, double& amax)
        {
            fA2F.getMappingValues(amin, amid, amax);
        }
};

//--------------------------------------------------------------------------------------
// Convert accelerometer or gyroscope values to Faust values
// Using an Up-Down curve (curve 2)
//--------------------------------------------------------------------------------------
class FAUST_API UpDownConverter : public UpdatableValueConverter {

    private:

        Interpolator3pt	fA2F;
        Interpolator fF2A;

    public:

        UpDownConverter(double amin, double amid, double amax, double fmin, double fmid, double fmax) :
            fA2F(amin,amid,amax,fmin,fmax,fmin),
            fF2A(fmin,fmax,amin,amax)				// Special, pseudo inverse of a non monotonic function
        {}

        virtual double ui2faust(double x) { return fA2F(x); }
        virtual double faust2ui(double x) { return fF2A(x); }

        virtual void setMappingValues(double amin, double amid, double amax, double fmin, double fmid, double fmax)
        {
            //__android_log_print(ANDROID_LOG_ERROR, "Faust", "UpDownConverter update %f %f %f %f %f %f", amin,amid,amax,fmin,fmid,fmax);
            fA2F = Interpolator3pt(amin, amid, amax, fmin, fmax, fmin);
            fF2A = Interpolator(fmin, fmax, amin, amax);
        }

        virtual void getMappingValues(double& amin, double& amid, double& amax)
        {
            fA2F.getMappingValues(amin, amid, amax);
        }
};

//--------------------------------------------------------------------------------------
// Convert accelerometer or gyroscope values to Faust values
// Using a Down-Up curve (curve 3)
//--------------------------------------------------------------------------------------
class FAUST_API DownUpConverter : public UpdatableValueConverter {

    private:

        Interpolator3pt	fA2F;
        Interpolator fF2A;

    public:

        DownUpConverter(double amin, double amid, double amax, double fmin, double fmid, double fmax) :
            fA2F(amin,amid,amax,fmax,fmin,fmax),
            fF2A(fmin,fmax,amin,amax)				// Special, pseudo inverse of a non monotonic function
        {}

        virtual double ui2faust(double x) { return fA2F(x); }
        virtual double faust2ui(double x) { return fF2A(x); }

        virtual void setMappingValues(double amin, double amid, double amax, double fmin, double fmid, double fmax)
        {
            //__android_log_print(ANDROID_LOG_ERROR, "Faust", "DownUpConverter update %f %f %f %f %f %f", amin,amid,amax,fmin,fmid,fmax);
            fA2F = Interpolator3pt(amin, amid, amax, fmax, fmin, fmax);
            fF2A = Interpolator(fmin, fmax, amin, amax);
        }

        virtual void getMappingValues(double& amin, double& amid, double& amax)
        {
            fA2F.getMappingValues(amin, amid, amax);
        }
};

//--------------------------------------------------------------------------------------
// Base class for ZoneControl
//--------------------------------------------------------------------------------------
class FAUST_API ZoneControl {

    protected:

        FAUSTFLOAT*	fZone;

    public:

        ZoneControl(FAUSTFLOAT* zone) : fZone(zone) {}
        virtual ~ZoneControl() {}

        virtual void update(double v) const {}

        virtual void setMappingValues(int curve, double amin, double amid, double amax, double min, double init, double max) {}
        virtual void getMappingValues(double& amin, double& amid, double& amax) {}

        FAUSTFLOAT* getZone() { return fZone; }

        virtual void setActive(bool on_off) {}
        virtual bool getActive() { return false; }

        virtual int getCurve() { return -1; }

};

//--------------------------------------------------------------------------------------
//  Useful to implement accelerometers metadata as a list of ZoneControl for each axes
//--------------------------------------------------------------------------------------
class FAUST_API ConverterZoneControl : public ZoneControl {

    protected:

        ValueConverter* fValueConverter;

    public:

        ConverterZoneControl(FAUSTFLOAT* zone, ValueConverter* converter) : ZoneControl(zone), fValueConverter(converter) {}
        virtual ~ConverterZoneControl() { delete fValueConverter; } // Assuming fValueConverter is not kept elsewhere...

        virtual void update(double v) const { *fZone = FAUSTFLOAT(fValueConverter->ui2faust(v)); }

        ValueConverter* getConverter() { return fValueConverter; }

};

//--------------------------------------------------------------------------------------
// Association of a zone and a four value converter, each one for each possible curve.
// Useful to implement accelerometers metadata as a list of ZoneControl for each axes
//--------------------------------------------------------------------------------------
class FAUST_API CurveZoneControl : public ZoneControl {

    private:

        std::vector<UpdatableValueConverter*> fValueConverters;
        int fCurve;

    public:

        CurveZoneControl(FAUSTFLOAT* zone, int curve, double amin, double amid, double amax, double min, double init, double max) : ZoneControl(zone), fCurve(0)
        {
            assert(curve >= 0 && curve <= 3);
            fValueConverters.push_back(new UpConverter(amin, amid, amax, min, init, max));
            fValueConverters.push_back(new DownConverter(amin, amid, amax, min, init, max));
            fValueConverters.push_back(new UpDownConverter(amin, amid, amax, min, init, max));
            fValueConverters.push_back(new DownUpConverter(amin, amid, amax, min, init, max));
            fCurve = curve;
        }
    
        virtual ~CurveZoneControl()
        {
            for (const auto& it : fValueConverters) { delete it; }
        }
    
        void update(double v) const { if (fValueConverters[fCurve]->getActive()) *fZone = FAUSTFLOAT(fValueConverters[fCurve]->ui2faust(v)); }

        void setMappingValues(int curve, double amin, double amid, double amax, double min, double init, double max)
        {
            fValueConverters[curve]->setMappingValues(amin, amid, amax, min, init, max);
            fCurve = curve;
        }

        void getMappingValues(double& amin, double& amid, double& amax)
        {
            fValueConverters[fCurve]->getMappingValues(amin, amid, amax);
        }

        void setActive(bool on_off)
        {
            for (const auto& it : fValueConverters) { it->setActive(on_off); }
        }

        int getCurve() { return fCurve; }
};

class FAUST_API ZoneReader {

    private:

        FAUSTFLOAT* fZone;
        Interpolator fInterpolator;

    public:

        ZoneReader(FAUSTFLOAT* zone, double lo, double hi) : fZone(zone), fInterpolator(lo, hi, 0, 255) {}

        virtual ~ZoneReader() {}

        int getValue()
        {
            return (fZone != nullptr) ? int(fInterpolator(*fZone)) : 127;
        }

};

#endif
/**************************  END  ValueConverter.h **************************/
/************************** BEGIN MetaDataUI.h **************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef MetaData_UI_H
#define MetaData_UI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <map>
#include <set>
#include <string>
#include <string.h>
#include <assert.h>
#include <stdio.h> // We use the lighter fprintf code

/************************** BEGIN SimpleParser.h *********************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ********************************************************************/

#ifndef SIMPLEPARSER_H
#define SIMPLEPARSER_H

// ---------------------------------------------------------------------
//                          Simple Parser
// A parser returns true if it was able to parse what it is
// supposed to parse and advance the pointer. Otherwise it returns false
// and the pointer is not advanced so that another parser can be tried.
// ---------------------------------------------------------------------

#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdio.h> // We use the lighter fprintf code
#include <ctype.h>
#include <assert.h>

#ifndef _WIN32
# pragma GCC diagnostic ignored "-Wunused-function"
#endif

struct itemInfo {
    std::string type;
    std::string label;
    std::string varname;
    std::string shortname;
    std::string address;
    std::string url;
    int index;
    double init;
    double fmin;
    double fmax;
    double step;
    std::vector<std::pair<std::string, std::string> > meta;
    
    itemInfo():index(0), init(0.), fmin(0.), fmax(0.), step(0.)
    {}
};

// ---------------------------------------------------------------------
//                          Elementary parsers
// ---------------------------------------------------------------------

// Report a parsing error
static bool parseError(const char*& p, const char* errmsg)
{
    fprintf(stderr, "Parse error : %s here : %s\n", errmsg, p);
    return true;
}

/**
 * @brief skipBlank : advance pointer p to the first non blank character
 * @param p the string to parse, then the remaining string
 */
static void skipBlank(const char*& p)
{
    while (isspace(*p)) { p++; }
}

// Parse character x, but don't report error if fails
static bool tryChar(const char*& p, char x)
{
    skipBlank(p);
    if (x == *p) {
        p++;
        return true;
    } else {
        return false;
    }
}

/**
 * @brief parseChar : parse a specific character x
 * @param p the string to parse, then the remaining string
 * @param x the character to recognize
 * @return true if x was found at the begin of p
 */
static bool parseChar(const char*& p, char x)
{
    skipBlank(p);
    if (x == *p) {
        p++;
        return true;
    } else {
        return false;
    }
}

/**
 * @brief parseWord : parse a specific string w
 * @param p the string to parse, then the remaining string
 * @param w the string to recognize
 * @return true if string w was found at the begin of p
 */
static bool parseWord(const char*& p, const char* w)
{
    skipBlank(p);
    const char* saved = p;  // to restore position if we fail
    while ((*w == *p) && (*w)) {++w; ++p;}
    if (*w) {
        p = saved;
        return false;
    } else {
        return true;
    }
}

/**
 * @brief parseDouble : parse number [s]dddd[.dddd] or [s]d[.dddd][E|e][s][dddd] and store the result in x
 * @param p the string to parse, then the remaining string
 * @param x the float number found if any
 * @return true if a float number was found at the begin of p
 */
static bool parseDouble(const char*& p, double& x)
{
    double sign = 1.0;     // sign of the number
    double ipart = 0;      // integral part of the number
    double dpart = 0;      // decimal part of the number before division
    double dcoef = 1.0;    // division factor for the decimal part
    double expsign = 1.0;  // sign of the E|e part
    double expcoef = 0.0;  // multiplication factor of E|e part
    
    bool valid = false;    // true if the number contains at least one digit
    
    skipBlank(p);
    const char* saved = p;  // to restore position if we fail
    
    // Sign
    if (parseChar(p, '+')) {
        sign = 1.0;
    } else if (parseChar(p, '-')) {
        sign = -1.0;
    }
    
    // Integral part
    while (isdigit(*p)) {
        valid = true;
        ipart = ipart*10 + (*p - '0');
        p++;
    }
    
    // Possible decimal part
    if (parseChar(p, '.')) {
        while (isdigit(*p)) {
            valid = true;
            dpart = dpart*10 + (*p - '0');
            dcoef *= 10.0;
            p++;
        }
    }
    
    // Possible E|e part
    if (parseChar(p, 'E') || parseChar(p, 'e')) {
        if (parseChar(p, '+')) {
            expsign = 1.0;
        } else if (parseChar(p, '-')) {
            expsign = -1.0;
        }
        while (isdigit(*p)) {
            expcoef = expcoef*10 + (*p - '0');
            p++;
        }
    }
    
    if (valid)  {
        x = (sign*(ipart + dpart/dcoef)) * std::pow(10.0, expcoef*expsign);
    } else {
        p = saved;
    }
    return valid;
}

/**
 * @brief parseString, parse an arbitrary quoted string q...q and store the result in s
 * @param p the string to parse, then the remaining string
 * @param quote the character used to quote the string
 * @param s the (unquoted) string found if any
 * @return true if a string was found at the begin of p
 */
static bool parseString(const char*& p, char quote, std::string& s)
{
    std::string str;
    skipBlank(p);
    
    const char* saved = p;  // to restore position if we fail
    if (*p++ == quote) {
        while ((*p != 0) && (*p != quote)) {
            str += *p++;
        }
        if (*p++ == quote) {
            s = str;
            return true;
        }
    }
    p = saved;
    return false;
}

/**
 * @brief parseSQString, parse a single quoted string '...' and store the result in s
 * @param p the string to parse, then the remaining string
 * @param s the (unquoted) string found if any
 * @return true if a string was found at the begin of p
 */
static bool parseSQString(const char*& p, std::string& s)
{
    return parseString(p, '\'', s);
}

/**
 * @brief parseDQString, parse a double quoted string "..." and store the result in s
 * @param p the string to parse, then the remaining string
 * @param s the (unquoted) string found if any
 * @return true if a string was found at the begin of p
 */
static bool parseDQString(const char*& p, std::string& s)
{
    return parseString(p, '"', s);
}

// ---------------------------------------------------------------------
//
//                          IMPLEMENTATION
// 
// ---------------------------------------------------------------------

/**
 * @brief parseMenuItem, parse a menu item ...'low':440.0...
 * @param p the string to parse, then the remaining string
 * @param name the name found
 * @param value the value found
 * @return true if a nemu item was found
 */
static bool parseMenuItem(const char*& p, std::string& name, double& value)
{
    const char* saved = p;  // to restore position if we fail
    if (parseSQString(p, name) && parseChar(p, ':') && parseDouble(p, value)) {
        return true;
    } else {
        p = saved;
        return false;
    }
}

static bool parseMenuItem2(const char*& p, std::string& name)
{
    const char* saved = p;  // to restore position if we fail
    // single quoted
    if (parseSQString(p, name)) {
        return true;
    } else {
        p = saved;
        return false;
    }
}

/**
 * @brief parseMenuList, parse a menu list {'low' : 440.0; 'mid' : 880.0; 'hi' : 1760.0}...
 * @param p the string to parse, then the remaining string
 * @param names the vector of names found
 * @param values the vector of values found
 * @return true if a menu list was found
 */
static bool parseMenuList(const char*& p, std::vector<std::string>& names, std::vector<double>& values)
{
    std::vector<std::string> tmpnames;
    std::vector<double> tmpvalues;
    const char* saved = p; // to restore position if we fail

    if (parseChar(p, '{')) {
        do {
            std::string n;
            double v;
            if (parseMenuItem(p, n, v)) {
                tmpnames.push_back(n);
                tmpvalues.push_back(v);
            } else {
                p = saved;
                return false;
            }
        } while (parseChar(p, ';'));
        if (parseChar(p, '}')) {
            // we suceeded
            names = tmpnames;
            values = tmpvalues;
            return true;
        }
    }
    p = saved;
    return false;
}

static bool parseMenuList2(const char*& p, std::vector<std::string>& names, bool debug)
{
    std::vector<std::string> tmpnames;
    const char* saved = p;  // to restore position if we fail
    
    if (parseChar(p, '{')) {
        do {
            std::string n;
            if (parseMenuItem2(p, n)) {
                tmpnames.push_back(n);
            } else {
                goto error;
            }
        } while (parseChar(p, ';'));
        if (parseChar(p, '}')) {
            // we suceeded
            names = tmpnames;
            return true;
        }
    }
    
error:
    if (debug) { fprintf(stderr, "parseMenuList2 : (%s) is not a valid list !\n", p); }
    p = saved;
    return false;
}

/// ---------------------------------------------------------------------
// Parse list of strings
/// ---------------------------------------------------------------------
static bool parseList(const char*& p, std::vector<std::string>& items)
{
    const char* saved = p;  // to restore position if we fail
    if (parseChar(p, '[')) {
        do {
            std::string item;
            if (!parseDQString(p, item)) {
                p = saved;
                return false;
            }
            items.push_back(item);
        } while (tryChar(p, ','));
        return parseChar(p, ']');
    } else {
        p = saved;
        return false;
    }
}

static bool parseMetaData(const char*& p, std::map<std::string, std::string>& metadatas)
{
    const char* saved = p; // to restore position if we fail
    std::string metaKey, metaValue;
    if (parseChar(p, ':') && parseChar(p, '[')) {
        do { 
            if (parseChar(p, '{') && parseDQString(p, metaKey) && parseChar(p, ':') && parseDQString(p, metaValue) && parseChar(p, '}')) {
                metadatas[metaKey] = metaValue;
            }
        } while (tryChar(p, ','));
        return parseChar(p, ']');
    } else {
        p = saved;
        return false;
    }
}

static bool parseItemMetaData(const char*& p, std::vector<std::pair<std::string, std::string> >& metadatas)
{
    const char* saved = p; // to restore position if we fail
    std::string metaKey, metaValue;
    if (parseChar(p, ':') && parseChar(p, '[')) {
        do { 
            if (parseChar(p, '{') && parseDQString(p, metaKey) && parseChar(p, ':') && parseDQString(p, metaValue) && parseChar(p, '}')) {
                metadatas.push_back(std::make_pair(metaKey, metaValue));
            }
        } while (tryChar(p, ','));
        return parseChar(p, ']');
    } else {
        p = saved;
        return false;
    }
}

// ---------------------------------------------------------------------
// Parse metadatas of the interface:
// "name" : "...", "inputs" : "...", "outputs" : "...", ...
// and store the result as key/value
/// ---------------------------------------------------------------------
static bool parseGlobalMetaData(const char*& p, std::string& key, std::string& value, double& dbl, std::map<std::string, std::string>& metadatas, std::vector<std::string>& items)
{
    const char* saved = p; // to restore position if we fail
    if (parseDQString(p, key)) {
        if (key == "meta") {
            return parseMetaData(p, metadatas);
        } else {
            return parseChar(p, ':') && (parseDQString(p, value) || parseList(p, items) || parseDouble(p, dbl));
        }
    } else {
        p = saved;
        return false;
    }
}

// ---------------------------------------------------------------------
// Parse gui:
// "type" : "...", "label" : "...", "address" : "...", ...
// and store the result in uiItems Vector
/// ---------------------------------------------------------------------
static bool parseUI(const char*& p, std::vector<itemInfo>& uiItems, int& numItems)
{
    const char* saved = p; // to restore position if we fail
    if (parseChar(p, '{')) {
   
        std::string label;
        std::string value;
        double dbl = 0;
        
        do {
            if (parseDQString(p, label)) {
                if (label == "type") {
                    if (uiItems.size() != 0) {
                        numItems++;
                    }
                    if (parseChar(p, ':') && parseDQString(p, value)) {   
                        itemInfo item;
                        item.type = value;
                        uiItems.push_back(item);
                    }
                }
                
                else if (label == "label") {
                    if (parseChar(p, ':') && parseDQString(p, value)) {
                        uiItems[numItems].label = value;
                    }
                }
                
                else if (label == "varname") {
                    if (parseChar(p, ':') && parseDQString(p, value)) {
                        uiItems[numItems].varname = value;
                    }
                }
                
                else if (label == "shortname") {
                    if (parseChar(p, ':') && parseDQString(p, value)) {
                        uiItems[numItems].shortname = value;
                    }
                }
                
                else if (label == "address") {
                    if (parseChar(p, ':') && parseDQString(p, value)) {
                        uiItems[numItems].address = value;
                    }
                }
                
                else if (label == "url") {
                    if (parseChar(p, ':') && parseDQString(p, value)) {
                        uiItems[numItems].url = value;
                    }
                }
                
                else if (label == "index") {
                    if (parseChar(p, ':') && parseDouble(p, dbl)) {
                        uiItems[numItems].index = int(dbl);
                    }
                }
                
                else if (label == "meta") {
                    if (!parseItemMetaData(p, uiItems[numItems].meta)) {
                        return false;
                    }
                }
                
                else if (label == "init") {
                    if (parseChar(p, ':') && parseDouble(p, dbl)) {
                        uiItems[numItems].init = dbl;
                    }
                }
                
                else if (label == "min") {
                    if (parseChar(p, ':') && parseDouble(p, dbl)) {
                        uiItems[numItems].fmin = dbl;
                    }
                }
                
                else if (label == "max") {
                    if (parseChar(p, ':') && parseDouble(p, dbl)) {
                        uiItems[numItems].fmax = dbl;
                    }
                }
                
                else if (label == "step") {
                    if (parseChar(p, ':') && parseDouble(p, dbl)) {
                        uiItems[numItems].step = dbl;
                    }
                }
                
                else if (label == "items") {
                    if (parseChar(p, ':') && parseChar(p, '[')) {
                        do {
                            if (!parseUI(p, uiItems, numItems)) {
                                p = saved;
                                return false;
                            }
                        } while (tryChar(p, ','));
                        if (parseChar(p, ']')) {
                            itemInfo item;
                            item.type = "close";
                            uiItems.push_back(item);
                            numItems++;
                        }
                    }
            
                } else {
                    fprintf(stderr, "Parse error unknown : %s \n", label.c_str());
                    assert(false);
                }
            } else {
                p = saved;
                return false;
            }
            
        } while (tryChar(p, ','));
    
        return parseChar(p, '}');
    } else {
        return true; // "items": [] is valid
    }
}

// ---------------------------------------------------------------------
// Parse full JSON record describing a JSON/Faust interface :
// {"metadatas": "...", "ui": [{ "type": "...", "label": "...", "items": [...], "address": "...","init": "...", "min": "...", "max": "...","step": "..."}]}
//
// and store the result in map Metadatas and vector containing the items of the interface. Returns true if parsing was successfull.
/// ---------------------------------------------------------------------
static bool parseJson(const char*& p,
                      std::map<std::string, std::pair<std::string, double> >& metaDatas0,
                      std::map<std::string, std::string>& metaDatas1,
                      std::map<std::string, std::vector<std::string> >& metaDatas2,
                      std::vector<itemInfo>& uiItems)
{
    parseChar(p, '{');
    
    do {
        std::string key;
        std::string value;
        double dbl = 0;
        std::vector<std::string> items;
        if (parseGlobalMetaData(p, key, value, dbl, metaDatas1, items)) {
            if (key != "meta") {
                // keep "name", "inputs", "outputs" key/value pairs
                if (items.size() > 0) {
                    metaDatas2[key] = items;
                    items.clear();
                } else if (value != "") {
                    metaDatas0[key].first = value;
                } else {
                    metaDatas0[key].second = dbl;
                }
            }
        } else if (key == "ui") {
            int numItems = 0;
            parseChar(p, '[') && parseUI(p, uiItems, numItems);
        }
    } while (tryChar(p, ','));
    
    return parseChar(p, '}');
}

#endif // SIMPLEPARSER_H
/**************************  END  SimpleParser.h **************************/

static bool startWith(const std::string& str, const std::string& prefix)
{
    return (str.substr(0, prefix.size()) == prefix);
}

/**
 * Convert a dB value into a scale between 0 and 1 (following IEC standard ?)
 */
static FAUSTFLOAT dB2Scale(FAUSTFLOAT dB)
{
    FAUSTFLOAT scale = FAUSTFLOAT(1.0);
    
    /*if (dB < -70.0f)
     scale = 0.0f;
     else*/
    if (dB < FAUSTFLOAT(-60.0))
        scale = (dB + FAUSTFLOAT(70.0)) * FAUSTFLOAT(0.0025);
    else if (dB < FAUSTFLOAT(-50.0))
        scale = (dB + FAUSTFLOAT(60.0)) * FAUSTFLOAT(0.005) + FAUSTFLOAT(0.025);
    else if (dB < FAUSTFLOAT(-40.0))
        scale = (dB + FAUSTFLOAT(50.0)) * FAUSTFLOAT(0.0075) + FAUSTFLOAT(0.075);
    else if (dB < FAUSTFLOAT(-30.0))
        scale = (dB + FAUSTFLOAT(40.0)) * FAUSTFLOAT(0.015) + FAUSTFLOAT(0.15);
    else if (dB < FAUSTFLOAT(-20.0))
        scale = (dB + FAUSTFLOAT(30.0)) * FAUSTFLOAT(0.02) + FAUSTFLOAT(0.3);
    else if (dB < FAUSTFLOAT(-0.001) || dB > FAUSTFLOAT(0.001))  /* if (dB < 0.0) */
        scale = (dB + FAUSTFLOAT(20.0)) * FAUSTFLOAT(0.025) + FAUSTFLOAT(0.5);
    
    return scale;
}

/*******************************************************************************
 * MetaDataUI : Common class for MetaData handling
 ******************************************************************************/

//============================= BEGIN GROUP LABEL METADATA===========================
// Unlike widget's label, metadata inside group's label are not extracted directly by
// the Faust compiler. Therefore they must be extracted within the architecture file
//-----------------------------------------------------------------------------------

class MetaDataUI {
    
    protected:
        
        std::string                         fGroupTooltip;
        std::map<FAUSTFLOAT*, FAUSTFLOAT>   fGuiSize;            // map widget zone with widget size coef
        std::map<FAUSTFLOAT*, std::string>  fTooltip;            // map widget zone with tooltip strings
        std::map<FAUSTFLOAT*, std::string>  fUnit;               // map widget zone to unit string (i.e. "dB")
        std::map<FAUSTFLOAT*, std::string>  fRadioDescription;   // map zone to {'low':440; ...; 'hi':1000.0}
        std::map<FAUSTFLOAT*, std::string>  fMenuDescription;    // map zone to {'low':440; ...; 'hi':1000.0}
        std::set<FAUSTFLOAT*>               fKnobSet;            // set of widget zone to be knobs
        std::set<FAUSTFLOAT*>               fLedSet;             // set of widget zone to be LEDs
        std::set<FAUSTFLOAT*>               fNumSet;             // set of widget zone to be numerical bargraphs
        std::set<FAUSTFLOAT*>               fLogSet;             // set of widget zone having a log UI scale
        std::set<FAUSTFLOAT*>               fExpSet;             // set of widget zone having an exp UI scale
        std::set<FAUSTFLOAT*>               fHiddenSet;          // set of hidden widget zone
        
        void clearMetadata()
        {
            fGuiSize.clear();
            fTooltip.clear();
            fUnit.clear();
            fRadioDescription.clear();
            fMenuDescription.clear();
            fKnobSet.clear();
            fLedSet.clear();
            fNumSet.clear();
            fLogSet.clear();
            fExpSet.clear();
            fHiddenSet.clear();
            fGroupTooltip = "";
        }
        
        /**
         * rmWhiteSpaces(): Remove the leading and trailing white spaces of a string
         * (but not those in the middle of the string)
         */
        static std::string rmWhiteSpaces(const std::string& s)
        {
            size_t i = s.find_first_not_of(" \t");
            size_t j = s.find_last_not_of(" \t");
            if ((i != std::string::npos) && (j != std::string::npos)) {
                return s.substr(i, 1+j-i);
            } else {
                return "";
            }
        }
        
        /**
         * Format tooltip string by replacing some white spaces by
         * return characters so that line width doesn't exceed n.
         * Limitation : long words exceeding n are not cut.
         */
        std::string formatTooltip(int n, const std::string& tt)
        {
            std::string ss = tt;  // ss string we are going to format
            int lws = 0;          // last white space encountered
            int lri = 0;          // last return inserted
            for (int i = 0; i < (int)tt.size(); i++) {
                if (tt[i] == ' ') lws = i;
                if (((i-lri) >= n) && (lws > lri)) {
                    // insert return here
                    ss[lws] = '\n';
                    lri = lws;
                }
            }
            return ss;
        }
        
    public:
        
        virtual ~MetaDataUI()
        {}
        
        enum Scale {
            kLin,
            kLog,
            kExp
        };
        
        Scale getScale(FAUSTFLOAT* zone)
        {
            if (fLogSet.count(zone) > 0) return kLog;
            if (fExpSet.count(zone) > 0) return kExp;
            return kLin;
        }
        
        bool isKnob(FAUSTFLOAT* zone)
        {
            return fKnobSet.count(zone) > 0;
        }
        
        bool isRadio(FAUSTFLOAT* zone)
        {
            return fRadioDescription.count(zone) > 0;
        }
        
        bool isMenu(FAUSTFLOAT* zone)
        {
            return fMenuDescription.count(zone) > 0;
        }
        
        bool isLed(FAUSTFLOAT* zone)
        {
            return fLedSet.count(zone) > 0;
        }
        
        bool isNumerical(FAUSTFLOAT* zone)
        {
            return fNumSet.count(zone) > 0;
        }
        
        bool isHidden(FAUSTFLOAT* zone)
        {
            return fHiddenSet.count(zone) > 0;
        }
        
        /**
         * Extracts metadata from a label : 'vol [unit: dB]' -> 'vol' + metadata(unit=dB)
         */
        static void extractMetadata(const std::string& fulllabel, std::string& label, std::map<std::string, std::string>& metadata)
        {
            enum {kLabel, kEscape1, kEscape2, kEscape3, kKey, kValue};
            int state = kLabel; int deep = 0;
            std::string key, value;
            
            for (unsigned int i = 0; i < fulllabel.size(); i++) {
                char c = fulllabel[i];
                switch (state) {
                    case kLabel :
                        assert(deep == 0);
                        switch (c) {
                            case '\\' : state = kEscape1; break;
                            case '[' : state = kKey; deep++; break;
                            default : label += c;
                        }
                        break;
                        
                    case kEscape1:
                        label += c;
                        state = kLabel;
                        break;
                        
                    case kEscape2:
                        key += c;
                        state = kKey;
                        break;
                        
                    case kEscape3:
                        value += c;
                        state = kValue;
                        break;
                        
                    case kKey:
                        assert(deep > 0);
                        switch (c) {
                            case '\\':
                                state = kEscape2;
                                break;
                                
                            case '[':
                                deep++;
                                key += c;
                                break;
                                
                            case ':':
                                if (deep == 1) {
                                    state = kValue;
                                } else {
                                    key += c;
                                }
                                break;
                            case ']':
                                deep--;
                                if (deep < 1) {
                                    metadata[rmWhiteSpaces(key)] = "";
                                    state = kLabel;
                                    key = "";
                                    value = "";
                                } else {
                                    key += c;
                                }
                                break;
                            default : key += c;
                        }
                        break;
                        
                    case kValue:
                        assert(deep > 0);
                        switch (c) {
                            case '\\':
                                state = kEscape3;
                                break;
                                
                            case '[':
                                deep++;
                                value += c;
                                break;
                                
                            case ']':
                                deep--;
                                if (deep < 1) {
                                    metadata[rmWhiteSpaces(key)] = rmWhiteSpaces(value);
                                    state = kLabel;
                                    key = "";
                                    value = "";
                                } else {
                                    value += c;
                                }
                                break;
                            default : value += c;
                        }
                        break;
                        
                    default:
                        fprintf(stderr, "ERROR unrecognized state %d\n", state);
                }
            }
            label = rmWhiteSpaces(label);
        }
        
        /**
         * Analyses the widget zone metadata declarations and takes appropriate actions.
         */
        void declare(FAUSTFLOAT* zone, const char* key, const char* value)
        {
            if (zone == 0) {
                // special zone 0 means group metadata
                if (strcmp(key, "tooltip") == 0) {
                    // only group tooltip are currently implemented
                    fGroupTooltip = formatTooltip(30, value);
                } else if ((strcmp(key, "hidden") == 0) && (strcmp(value, "1") == 0)) {
                    fHiddenSet.insert(zone);
                }
            } else {
                if (strcmp(key, "size") == 0) {
                    fGuiSize[zone] = atof(value);
                }
                else if (strcmp(key, "tooltip") == 0) {
                    fTooltip[zone] = formatTooltip(30, value);
                }
                else if (strcmp(key, "unit") == 0) {
                    fUnit[zone] = value;
                }
                else if ((strcmp(key, "hidden") == 0) && (strcmp(value, "1") == 0)) {
                    fHiddenSet.insert(zone);
                }
                else if (strcmp(key, "scale") == 0) {
                    if (strcmp(value, "log") == 0) {
                        fLogSet.insert(zone);
                    } else if (strcmp(value, "exp") == 0) {
                        fExpSet.insert(zone);
                    }
                }
                else if (strcmp(key, "style") == 0) {
                    if (strcmp(value, "knob") == 0) {
                        fKnobSet.insert(zone);
                    } else if (strcmp(value, "led") == 0) {
                        fLedSet.insert(zone);
                    } else if (strcmp(value, "numerical") == 0) {
                        fNumSet.insert(zone);
                    } else {
                        const char* p = value;
                        if (parseWord(p, "radio")) {
                            fRadioDescription[zone] = std::string(p);
                        } else if (parseWord(p, "menu")) {
                            fMenuDescription[zone] = std::string(p);
                        }
                    }
                }
            }
        }
    
};

#endif
/**************************  END  MetaDataUI.h **************************/
/************************** BEGIN ring-buffer.h **************************/
/*
  Copyright (C) 2000 Paul Davis
  Copyright (C) 2003 Rohan Drape
  Copyright (C) 2016 GRAME (renaming for internal use)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

  ISO/POSIX C version of Paul Davis's lock free ringbuffer C++ code.
  This is safe for the case of one read thread and one write thread.
*/

#ifndef __ring_buffer__
#define __ring_buffer__

#include <stdlib.h>
#include <string.h>

#ifdef WIN32
# pragma warning (disable: 4334)
#else
# pragma GCC diagnostic ignored "-Wunused-function"
#endif

typedef struct {
    char *buf;
    size_t len;
}
ringbuffer_data_t;

typedef struct {
    char *buf;
    volatile size_t write_ptr;
    volatile size_t read_ptr;
    size_t	size;
    size_t	size_mask;
    int	mlocked;
}
ringbuffer_t;

static ringbuffer_t *ringbuffer_create(size_t sz);
static void ringbuffer_free(ringbuffer_t *rb);
static void ringbuffer_get_read_vector(const ringbuffer_t *rb,
                                         ringbuffer_data_t *vec);
static void ringbuffer_get_write_vector(const ringbuffer_t *rb,
                                          ringbuffer_data_t *vec);
static size_t ringbuffer_read(ringbuffer_t *rb, char *dest, size_t cnt);
static size_t ringbuffer_peek(ringbuffer_t *rb, char *dest, size_t cnt);
static void ringbuffer_read_advance(ringbuffer_t *rb, size_t cnt);
static size_t ringbuffer_read_space(const ringbuffer_t *rb);
static int ringbuffer_mlock(ringbuffer_t *rb);
static void ringbuffer_reset(ringbuffer_t *rb);
static void ringbuffer_reset_size (ringbuffer_t * rb, size_t sz);
static size_t ringbuffer_write(ringbuffer_t *rb, const char *src,
                                 size_t cnt);
static void ringbuffer_write_advance(ringbuffer_t *rb, size_t cnt);
static size_t ringbuffer_write_space(const ringbuffer_t *rb);

/* Create a new ringbuffer to hold at least `sz' bytes of data. The
   actual buffer size is rounded up to the next power of two. */

static ringbuffer_t *
ringbuffer_create (size_t sz)
{
	size_t power_of_two;
	ringbuffer_t *rb;

	if ((rb = (ringbuffer_t *) malloc (sizeof (ringbuffer_t))) == NULL) {
		return NULL;
	}

	for (power_of_two = 1u; 1u << power_of_two < sz; power_of_two++);

	rb->size = 1u << power_of_two;
	rb->size_mask = rb->size;
	rb->size_mask -= 1;
	rb->write_ptr = 0;
	rb->read_ptr = 0;
	if ((rb->buf = (char *) malloc (rb->size)) == NULL) {
		free (rb);
		return NULL;
	}
	rb->mlocked = 0;

	return rb;
}

/* Free all data associated with the ringbuffer `rb'. */

static void
ringbuffer_free (ringbuffer_t * rb)
{
#ifdef USE_MLOCK
	if (rb->mlocked) {
		munlock (rb->buf, rb->size);
	}
#endif /* USE_MLOCK */
	free (rb->buf);
	free (rb);
}

/* Lock the data block of `rb' using the system call 'mlock'.  */

static int
ringbuffer_mlock (ringbuffer_t * rb)
{
#ifdef USE_MLOCK
	if (mlock (rb->buf, rb->size)) {
		return -1;
	}
#endif /* USE_MLOCK */
	rb->mlocked = 1;
	return 0;
}

/* Reset the read and write pointers to zero. This is not thread
   safe. */

static void
ringbuffer_reset (ringbuffer_t * rb)
{
	rb->read_ptr = 0;
	rb->write_ptr = 0;
    memset(rb->buf, 0, rb->size);
}

/* Reset the read and write pointers to zero. This is not thread
   safe. */

static void
ringbuffer_reset_size (ringbuffer_t * rb, size_t sz)
{
    rb->size = sz;
    rb->size_mask = rb->size;
    rb->size_mask -= 1;
    rb->read_ptr = 0;
    rb->write_ptr = 0;
}

/* Return the number of bytes available for reading. This is the
   number of bytes in front of the read pointer and behind the write
   pointer.  */

static size_t
ringbuffer_read_space (const ringbuffer_t * rb)
{
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r) {
		return w - r;
	} else {
		return (w - r + rb->size) & rb->size_mask;
	}
}

/* Return the number of bytes available for writing. This is the
   number of bytes in front of the write pointer and behind the read
   pointer.  */

static size_t
ringbuffer_write_space (const ringbuffer_t * rb)
{
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r) {
		return ((r - w + rb->size) & rb->size_mask) - 1;
	} else if (w < r) {
		return (r - w) - 1;
	} else {
		return rb->size - 1;
	}
}

/* The copying data reader. Copy at most `cnt' bytes from `rb' to
   `dest'.  Returns the actual number of bytes copied. */

static size_t
ringbuffer_read (ringbuffer_t * rb, char *dest, size_t cnt)
{
	size_t free_cnt;
	size_t cnt2;
	size_t to_read;
	size_t n1, n2;

	if ((free_cnt = ringbuffer_read_space (rb)) == 0) {
		return 0;
	}

	to_read = cnt > free_cnt ? free_cnt : cnt;

	cnt2 = rb->read_ptr + to_read;

	if (cnt2 > rb->size) {
		n1 = rb->size - rb->read_ptr;
		n2 = cnt2 & rb->size_mask;
	} else {
		n1 = to_read;
		n2 = 0;
	}

	memcpy (dest, &(rb->buf[rb->read_ptr]), n1);
	rb->read_ptr = (rb->read_ptr + n1) & rb->size_mask;

	if (n2) {
		memcpy (dest + n1, &(rb->buf[rb->read_ptr]), n2);
		rb->read_ptr = (rb->read_ptr + n2) & rb->size_mask;
	}

	return to_read;
}

/* The copying data reader w/o read pointer advance. Copy at most
   `cnt' bytes from `rb' to `dest'.  Returns the actual number of bytes
   copied. */

static size_t
ringbuffer_peek (ringbuffer_t * rb, char *dest, size_t cnt)
{
	size_t free_cnt;
	size_t cnt2;
	size_t to_read;
	size_t n1, n2;
	size_t tmp_read_ptr;

	tmp_read_ptr = rb->read_ptr;

	if ((free_cnt = ringbuffer_read_space (rb)) == 0) {
		return 0;
	}

	to_read = cnt > free_cnt ? free_cnt : cnt;

	cnt2 = tmp_read_ptr + to_read;

	if (cnt2 > rb->size) {
		n1 = rb->size - tmp_read_ptr;
		n2 = cnt2 & rb->size_mask;
	} else {
		n1 = to_read;
		n2 = 0;
	}

	memcpy (dest, &(rb->buf[tmp_read_ptr]), n1);
	tmp_read_ptr = (tmp_read_ptr + n1) & rb->size_mask;

	if (n2) {
		memcpy (dest + n1, &(rb->buf[tmp_read_ptr]), n2);
	}

	return to_read;
}

/* The copying data writer. Copy at most `cnt' bytes to `rb' from
   `src'.  Returns the actual number of bytes copied. */

static size_t
ringbuffer_write (ringbuffer_t * rb, const char *src, size_t cnt)
{
	size_t free_cnt;
	size_t cnt2;
	size_t to_write;
	size_t n1, n2;

	if ((free_cnt = ringbuffer_write_space (rb)) == 0) {
		return 0;
	}

	to_write = cnt > free_cnt ? free_cnt : cnt;

	cnt2 = rb->write_ptr + to_write;

	if (cnt2 > rb->size) {
		n1 = rb->size - rb->write_ptr;
		n2 = cnt2 & rb->size_mask;
	} else {
		n1 = to_write;
		n2 = 0;
	}

	memcpy (&(rb->buf[rb->write_ptr]), src, n1);
	rb->write_ptr = (rb->write_ptr + n1) & rb->size_mask;

	if (n2) {
		memcpy (&(rb->buf[rb->write_ptr]), src + n1, n2);
		rb->write_ptr = (rb->write_ptr + n2) & rb->size_mask;
	}

	return to_write;
}

/* Advance the read pointer `cnt' places. */

static void
ringbuffer_read_advance (ringbuffer_t * rb, size_t cnt)
{
	size_t tmp = (rb->read_ptr + cnt) & rb->size_mask;
	rb->read_ptr = tmp;
}

/* Advance the write pointer `cnt' places. */

static void
ringbuffer_write_advance (ringbuffer_t * rb, size_t cnt)
{
	size_t tmp = (rb->write_ptr + cnt) & rb->size_mask;
	rb->write_ptr = tmp;
}

/* The non-copying data reader. `vec' is an array of two places. Set
   the values at `vec' to hold the current readable data at `rb'. If
   the readable data is in one segment the second segment has zero
   length. */

static void
ringbuffer_get_read_vector (const ringbuffer_t * rb,
				 ringbuffer_data_t * vec)
{
	size_t free_cnt;
	size_t cnt2;
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r) {
		free_cnt = w - r;
	} else {
		free_cnt = (w - r + rb->size) & rb->size_mask;
	}

	cnt2 = r + free_cnt;

	if (cnt2 > rb->size) {

		/* Two part vector: the rest of the buffer after the current write
		   ptr, plus some from the start of the buffer. */

		vec[0].buf = &(rb->buf[r]);
		vec[0].len = rb->size - r;
		vec[1].buf = rb->buf;
		vec[1].len = cnt2 & rb->size_mask;

	} else {

		/* Single part vector: just the rest of the buffer */

		vec[0].buf = &(rb->buf[r]);
		vec[0].len = free_cnt;
		vec[1].len = 0;
	}
}

/* The non-copying data writer. `vec' is an array of two places. Set
   the values at `vec' to hold the current writeable data at `rb'. If
   the writeable data is in one segment the second segment has zero
   length. */

static void
ringbuffer_get_write_vector (const ringbuffer_t * rb,
				  ringbuffer_data_t * vec)
{
	size_t free_cnt;
	size_t cnt2;
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r) {
		free_cnt = ((r - w + rb->size) & rb->size_mask) - 1;
	} else if (w < r) {
		free_cnt = (r - w) - 1;
	} else {
		free_cnt = rb->size - 1;
	}

	cnt2 = w + free_cnt;

	if (cnt2 > rb->size) {

		/* Two part vector: the rest of the buffer after the current write
		   ptr, plus some from the start of the buffer. */

		vec[0].buf = &(rb->buf[w]);
		vec[0].len = rb->size - w;
		vec[1].buf = rb->buf;
		vec[1].len = cnt2 & rb->size_mask;
	} else {
		vec[0].buf = &(rb->buf[w]);
		vec[0].len = free_cnt;
		vec[1].len = 0;
	}
}

#endif // __ring_buffer__
/**************************  END  ring-buffer.h **************************/

/*******************************************************************************
 * GUI : Abstract Graphic User Interface
 * Provides additional mechanisms to synchronize widgets and zones. Widgets
 * should both reflect the value of a zone and allow to change this value.
 ******************************************************************************/

class uiItem;
class GUI;
struct clist;

typedef void (*uiCallback)(FAUSTFLOAT val, void* data);

/**
 * Base class for uiTypedItem: memory zones that can be grouped and synchronized, using an internal cache.
 */
struct uiItemBase
{
    
    uiItemBase(GUI* ui, FAUSTFLOAT* zone)
    {
        assert(ui);
        assert(zone);
    }
    
    virtual ~uiItemBase()
    {}
    
    /**
     * This method will be called when the value changes externally,
     * and will signal the new value to all linked uItem
     * when the value is different from the cached one.
     *
     * @param v - the new value
     */
    virtual void modifyZone(FAUSTFLOAT v) = 0;
    
    /**
     * This method will be called when the value changes externally,
     * and will signal the new value to all linked uItem
     * when the value is different from the cached one.
     *
     * @param date - the timestamp of the received value in usec
     * @param v - the new value
     */
    virtual void modifyZone(double date, FAUSTFLOAT v) {}
    
    /**
     * This method is called by the synchronisation mecanism and is expected
     * to 'reflect' the new value, by changing the Widget layout for instance,
     * or sending a message (OSC, MIDI...)
     */
    virtual void reflectZone() = 0;
    
    /**
     * Return the cached value.
     *
     * @return - the cached value
     */
    virtual double cache() = 0;
    
};

// Declared as 'static' to avoid code duplication at link time
static void deleteClist(clist* cl);

/**
 * A list containing all groupe uiItemBase objects.
 */
struct clist : public std::list<uiItemBase*>
{
    
    virtual ~clist()
    {
        deleteClist(this);
    }
        
};

static void createUiCallbackItem(GUI* ui, FAUSTFLOAT* zone, uiCallback foo, void* data);

typedef std::map<FAUSTFLOAT*, clist*> zmap;

typedef std::map<FAUSTFLOAT*, ringbuffer_t*> ztimedmap;

class GUI : public UI
{
		
    private:
     
        static std::list<GUI*> fGuiList;
        zmap fZoneMap;
        bool fStopped;
    
     public:
            
        GUI():fStopped(false)
        {	
            fGuiList.push_back(this);
        }
        
        virtual ~GUI() 
        {   
            // delete all items
            for (const auto& it : fZoneMap) {
                delete it.second;
            }
            // suppress 'this' in static fGuiList
            fGuiList.remove(this);
        }

        // -- registerZone(z,c) : zone management
        
        void registerZone(FAUSTFLOAT* z, uiItemBase* c)
        {
            if (fZoneMap.find(z) == fZoneMap.end()) fZoneMap[z] = new clist();
            fZoneMap[z]->push_back(c);
        }
    
        void updateZone(FAUSTFLOAT* z)
        {
            FAUSTFLOAT v = *z;
            clist* cl = fZoneMap[z];
            for (const auto& c : *cl) {
                if (c->cache() != v) c->reflectZone();
            }
        }
    
        void updateAllZones()
        {
            for (const auto& m : fZoneMap) {
                updateZone(m.first);
            }
        }
    
        // To be implemented when a single global update is needed per updateAllGuis refresh
        virtual void updateAll()
        {}
        
        static void updateAllGuis()
        {
            for (const auto& g : fGuiList) {
                g->updateAllZones();
                g->updateAll();
            }
        }
    
        void addCallback(FAUSTFLOAT* zone, uiCallback foo, void* data)
        {
            createUiCallbackItem(this, zone, foo, data);
        }

        // Start event or message processing
        virtual bool run() { return false; };
        // Stop event or message processing
        virtual void stop() { fStopped = true; }
        bool stopped() { return fStopped; }
    
        // -- widget's layouts
        
        virtual void openTabBox(const char* label) {}
        virtual void openHorizontalBox(const char* label) {}
        virtual void openVerticalBox(const char* label) {}
        virtual void closeBox() {}
        
        // -- active widgets
        
        virtual void addButton(const char* label, FAUSTFLOAT* zone) {}
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) {}
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {}
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {}
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {}
    
        // -- passive widgets
        
        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
    
        // -- soundfiles
    
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}
    
        // -- metadata declarations

        virtual void declare(FAUSTFLOAT*, const char*, const char*) {}
    
        // Static global for timed zones, shared between all UI that will set timed values
        static ztimedmap gTimedZoneMap;

};

/**
 * User Interface Item: abstract definition.
 */
template <typename REAL>
class uiTypedItemReal : public uiItemBase
{
    protected:
        
        GUI* fGUI;
        REAL* fZone;
        REAL fCache;
        
        uiTypedItemReal(GUI* ui, REAL* zone):uiItemBase(ui, static_cast<FAUSTFLOAT*>(zone)),
        fGUI(ui), fZone(zone), fCache(REAL(-123456.654321))
        {
            ui->registerZone(zone, this);
        }
        
    public:
        
        virtual ~uiTypedItemReal()
        {}
    
        void modifyZone(REAL v)
        {
            fCache = v;
            if (*fZone != v) {
                *fZone = v;
                fGUI->updateZone(fZone);
            }
        }
    
        double cache() { return fCache; }
    
};

class uiItem : public uiTypedItemReal<FAUSTFLOAT> {
    
    protected:
    
        uiItem(GUI* ui, FAUSTFLOAT* zone):uiTypedItemReal<FAUSTFLOAT>(ui, zone)
        {}

    public:

        virtual ~uiItem() 
        {}

		void modifyZone(FAUSTFLOAT v)
		{
			fCache = v;
			if (*fZone != v) {
				*fZone = v;
				fGUI->updateZone(fZone);
			}
		}

};

/**
 * Base class for items with a value converter.
 */
struct uiConverter {
    
    ValueConverter* fConverter;
    
    uiConverter(MetaDataUI::Scale scale, FAUSTFLOAT umin, FAUSTFLOAT umax, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
    {
        // Select appropriate converter according to scale mode
        if (scale == MetaDataUI::kLog) {
            fConverter = new LogValueConverter(umin, umax, fmin, fmax);
        } else if (scale == MetaDataUI::kExp) {
            fConverter = new ExpValueConverter(umin, umax, fmin, fmax);
        } else {
            fConverter = new LinearValueConverter(umin, umax, fmin, fmax);
        }
    }
    
    virtual ~uiConverter()
    {
        delete fConverter;
    }
};

/**
 * User Interface item owned (and so deleted) by external code.
 */
class uiOwnedItem : public uiItem {
    
    protected:
    
        uiOwnedItem(GUI* ui, FAUSTFLOAT* zone):uiItem(ui, zone)
        {}
    
     public:
    
        virtual ~uiOwnedItem()
        {}
    
        virtual void reflectZone() {}
};

/**
 * Callback Item.
 */
class uiCallbackItem : public uiItem {
    
    protected:
    
        uiCallback fCallback;
        void* fData;
    
    public:
    
        uiCallbackItem(GUI* ui, FAUSTFLOAT* zone, uiCallback foo, void* data)
        : uiItem(ui, zone), fCallback(foo), fData(data) {}
        
        virtual void reflectZone() 
        {		
            FAUSTFLOAT v = *fZone;
            fCache = v; 
            fCallback(v, fData);	
        }
};

/**
 *  For timestamped control.
 */
struct DatedControl {
    
    double fDate;
    FAUSTFLOAT fValue;
    
    DatedControl(double d = 0., FAUSTFLOAT v = FAUSTFLOAT(0)):fDate(d), fValue(v) {}
    
};

/**
 * Base class for timed items.
 */
class uiTimedItem : public uiItem
{
    
    protected:
        
        bool fDelete;
        
    public:
    
        using uiItem::modifyZone;
        
        uiTimedItem(GUI* ui, FAUSTFLOAT* zone):uiItem(ui, zone)
        {
            if (GUI::gTimedZoneMap.find(fZone) == GUI::gTimedZoneMap.end()) {
                GUI::gTimedZoneMap[fZone] = ringbuffer_create(8192);
                fDelete = true;
            } else {
                fDelete = false;
            }
        }
        
        virtual ~uiTimedItem()
        {
            ztimedmap::iterator it;
            if (fDelete && ((it = GUI::gTimedZoneMap.find(fZone)) != GUI::gTimedZoneMap.end())) {
                ringbuffer_free((*it).second);
                GUI::gTimedZoneMap.erase(it);
            }
        }
        
        virtual void modifyZone(double date, FAUSTFLOAT v)
        {
            size_t res;
            DatedControl dated_val(date, v);
            if ((res = ringbuffer_write(GUI::gTimedZoneMap[fZone], (const char*)&dated_val, sizeof(DatedControl))) != sizeof(DatedControl)) {
                fprintf(stderr, "ringbuffer_write error DatedControl\n");
            }
        }
    
};

/**
 * Allows to group a set of zones.
 */
class uiGroupItem : public uiItem
{
    protected:
    
        std::vector<FAUSTFLOAT*> fZoneMap;

    public:
    
        uiGroupItem(GUI* ui, FAUSTFLOAT* zone):uiItem(ui, zone)
        {}
        virtual ~uiGroupItem() 
        {}
        
        virtual void reflectZone() 
        {
            FAUSTFLOAT v = *fZone;
            fCache = v;
            
            // Update all zones of the same group
            for (const auto& it : fZoneMap) {
                *it = v;
            }
        }
        
        void addZone(FAUSTFLOAT* zone) { fZoneMap.push_back(zone); }

};

// Cannot be defined as method in the classes.

static void createUiCallbackItem(GUI* ui, FAUSTFLOAT* zone, uiCallback foo, void* data)
{
    new uiCallbackItem(ui, zone, foo, data);
}

static void deleteClist(clist* cl)
{
    for (const auto& it : *cl) {
        // This specific code is only used in JUCE context. TODO: use proper 'shared_ptr' based memory management.
    #if defined(JUCE_32BIT) || defined(JUCE_64BIT)
        uiOwnedItem* owned = dynamic_cast<uiOwnedItem*>(it);
        // owned items are deleted by external code
        if (!owned) {
            delete it;
        }
    #else
        delete it;
    #endif
    }
}

#endif
/**************************  END  GUI.h **************************/

/******************************************************************************
 *******************************************************************************
 
 USER INTERFACE
 
 *******************************************************************************
 *******************************************************************************/

#define NO_FILE             0
#define OUTPUT_FILE         1
#define INPUT_OUTPUT_FILE   2

class CMDUI : public UI
{
    private:
    
        std::thread* fThread;
        std::atomic<bool> fRun;
    
        struct param {
            FAUSTFLOAT* fZone; FAUSTFLOAT fMin; FAUSTFLOAT fMax;
            param(FAUSTFLOAT* z, FAUSTFLOAT init, FAUSTFLOAT a, FAUSTFLOAT b) : fZone(z), fMin(a), fMax(b) { *z = init; }
        };
    
        int                           fArgc;
        char**                        fArgv;
        std::vector<char*>            fFiles;
        std::stack<std::string>       fPrefix;
        std::map<std::string, param>  fKeyParam;
        bool                          fIgnoreParam;
    
        void openAnyBox(const char* label)
        {
            std::string prefix;
            
            if (label && label[0]) {
                prefix = fPrefix.top() + "-" + label;
            } else {
                prefix = fPrefix.top();
            }
            fPrefix.push(prefix);
        }
    
        std::string simplify(const std::string& src)
        {
            int i = 0;
            int level = 0;
            std::string dst;
            
            while (src[i]) {
                
                switch (level) {
                        
                    case 0 :
                    case 1 :
                    case 2 :
                        // Skip the begin of the label "--foo-" until 3 '-' have been read
                        if (src[i] == '-') { level++; }
                        break;
                        
                    case 3 :
                        // copy the content, but skip non alphanum and content in parenthesis
                        switch (src[i]) {
                            case '(' :
                            case '[' :
                                level++;
                                break;
                            case '-' :
                                dst += '-';
                                break;
                            default :
                                if (isalnum(src[i])) {
                                    dst+= tolower(src[i]);
                                }
                        }
                        break;
                        
                    default :
                        // here we are inside parenthesis and we skip the content until we are back to level 3
                        switch (src[i]) {
                            case '(' :
                            case '[' :
                                level++;
                                break;
                            case ')' :
                            case ']' :
                                level--;
                                break;
                            default :
                                break;
                        }
                }
                i++;
            }
            return dst;
        }
    
    public:
    
        CMDUI(int argc, char* argv[], bool ignore_param = false)
        : UI(), fThread(nullptr), fRun(true),
        fArgc(argc), fArgv(argv),
        fIgnoreParam(ignore_param)
        {
            fPrefix.push("-");
        }
    
        virtual ~CMDUI() { delete fThread; }
    
        // Can be used as public method
        void addOption(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max)
        {
            std::string fullname = "-" + simplify(fPrefix.top() + "-" + label);
            fKeyParam.insert(make_pair(fullname, param(zone, init, min, max)));
        }
    
        virtual void openTabBox(const char* label) { openAnyBox(label); }
        virtual void openHorizontalBox(const char* label) { openAnyBox(label); }
        virtual void openVerticalBox(const char* label) { openAnyBox(label); }
        virtual void closeBox() { fPrefix.pop(); }
    
        virtual void addButton(const char* label, FAUSTFLOAT* zone)
        {
            addOption(label, zone, 0, 0, 1);
        }
    
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            addOption(label, zone, 0, 0, 1);
        }
    
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            addOption(label, zone, init, min, max);
        }
    
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            addOption(label, zone, init, min, max);
        }
    
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            addOption(label, zone, init, min, max);
        }
    
        // -- passive widgets
    
        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
    
        // -- soundfiles
    
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}
    
        static void update_gui(CMDUI* ui)
        {
            while (ui->fRun) {
                usleep(40 * 1000); // 25Hz
                GUI::updateAllGuis();
            }
        }
    
        virtual bool run()
        {
            fThread = new std::thread(update_gui, this);
            std::cout << "Type 'q' to quit\n";
            char c;
            while ((c = getchar()) && (c != 'q')) {
                usleep(100 * 1000); 
            }
            fRun = false;
            fThread->join();
            return true;
        }
    
        void printhelp_command(int file_mode = NO_FILE)
        {
            if ((file_mode == OUTPUT_FILE && files() < 1)
                || (file_mode == INPUT_OUTPUT_FILE && files() < 2)) {
                std::cout << fArgv[0] << " ";
                for (const auto& i : fKeyParam) {
                    std::cout << "[ " << i.first << " " << i.second.fMin << ".." << i.second.fMax <<" ] ";
                }
                if (file_mode == INPUT_OUTPUT_FILE) {
                    std::cout << "infile outfile\n";
                } else if (file_mode == OUTPUT_FILE) {
                    std::cout << "outfile\n";
                }
                exit(1);
            }
        }
    
        void printhelp_init()
        {
            std::cout << fArgv[0] << " option list : ";
            for (const auto& i : fKeyParam) {
                std::cout << "[ " << i.first << " " << i.second.fMin << ".." << i.second.fMax <<" ] ";
            }
            std::cout << std::endl;
            exit(1);
        }
    
        void process_command(int file_mode = NO_FILE)
        {
            std::map<std::string, param>::iterator p;
            for (int i = 1; i < fArgc; i++) {
                if (fArgv[i][0] == '-') {
                    if ((strcmp(fArgv[i], "-help") == 0) || (strcmp(fArgv[i], "-h") == 0)) {
                        printhelp_command(file_mode);
                    }
                    p = fKeyParam.find(fArgv[i]);
                    if (p == fKeyParam.end()) {
                        if (!fIgnoreParam) {
                            std::cout << fArgv[0] << " : unrecognized option " << fArgv[i] << "\n";
                            printhelp_command(file_mode);
                        } else {
                            // Argument with a value, so also ignore the value
                            if ((i+1 < fArgc) && (fArgv[i+1][0] != '-')) i++;
                        }
                    } else {
                        char* end;
                        *(p->second.fZone) = FAUSTFLOAT(std::strtod(fArgv[i+1], &end));
                        i++;
                    }
                } else {
                    fFiles.push_back(fArgv[i]);
                }
            }
        }
    
        void process_init()
        {
            std::map<std::string, param>::iterator p;
            for (int i = 1; i < fArgc; i++) {
                if (fArgv[i][0] == '-') {
                    if ((strcmp(fArgv[i], "-help") == 0)
                        || (strcmp(fArgv[i], "-h") == 0)
                        || (strcmp(fArgv[i], "--help") == 0)) {
                        printhelp_init();
                    }
                    p = fKeyParam.find(fArgv[i]);
                    if (p == fKeyParam.end()) {
                        std::cout << fArgv[0] << " : unrecognized option " << fArgv[i] << "\n";
                        printhelp_init();
                    }
                    char* end;
                    *(p->second.fZone) = FAUSTFLOAT(std::strtod(fArgv[i+1], &end));
                    i++;
                }
            }
        }
    
        // check if exist and process one single parameter
        void process_one_init(const char* param1)
        {
            for (int i = 1; i < fArgc; i++) {
                if (fArgv[i][0] == '-') {
                    std::map<std::string, param>::iterator p = fKeyParam.find(fArgv[i]);
                    if ((p != fKeyParam.end()) && (strcmp(fArgv[i], param1) == 0)) {
                        char* end;
                        *(p->second.fZone) = FAUSTFLOAT(std::strtod(fArgv[i+1], &end));
                        break;
                    }
                    i++;
                }
            }
        }
    
        unsigned long files() { return fFiles.size(); }
        char* file(int n) { return fFiles[n]; }
    
        char* input_file() { return fFiles[0]; }
        char* output_file() { return fFiles[1]; }
    
};

#endif

/**************************  END  console.h **************************/
/************************** BEGIN FUI.h *****************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ***********************************************************************/

#ifndef FAUST_FUI_H
#define FAUST_FUI_H

#include <string>
#include <map>
#include <set>
#include <vector>
#include <stack>
#include <iostream>
#include <fstream>

/************************** BEGIN PathBuilder.h **************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __PathBuilder__
#define __PathBuilder__

#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>


/*******************************************************************************
 * PathBuilder : Faust User Interface
 * Helper class to build complete hierarchical path for UI items.
 ******************************************************************************/

class FAUST_API PathBuilder {

    protected:
    
        std::vector<std::string> fControlsLevel;
        std::vector<std::string> fFullPaths;
        std::map<std::string, std::string> fFull2Short;  // filled by computeShortNames()
    
        /**
         * @brief check if a character is acceptable for an ID
         *
         * @param c
         * @return true is the character is acceptable for an ID
         */
        bool isIDChar(char c) const
        {
            return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9'));
        }
    
        /**
         * @brief remove all "/0x00" parts
         *
         * @param src
         * @return modified string
         */
        std::string remove0x00(const std::string& src_aux) const
        {
            std::string src = src_aux;
            std::string from = "/0x00";
            std::string to = "";
            size_t pos = std::string::npos;
            while ((pos = src.find(from)) && (pos != std::string::npos)) {
                src = src.replace(pos, from.length(), to);
            }
            return src;
        }
    
        /**
         * @brief replace all non ID char with '_' (one '_' may replace several non ID char)
         *
         * @param src
         * @return modified string
         */
        std::string str2ID(const std::string& src) const
        {
            std::string dst;
            bool need_underscore = false;
            for (char c : src) {
                if (isIDChar(c) || (c == '/')) {
                    if (need_underscore) {
                        dst.push_back('_');
                        need_underscore = false;
                    }
                    dst.push_back(c);
                } else {
                    need_underscore = true;
                }
            }
            return dst;
        }
    
        /**
         * @brief Keep only the last n slash-parts
         *
         * @param src
         * @param n : 1 indicates the last slash-part
         * @return modified string
         */
        std::string cut(const std::string& src, int n) const
        {
            std::string rdst;
            for (int i = int(src.length())-1; i >= 0; i--) {
                char c = src[i];
                if (c != '/') {
                    rdst.push_back(c);
                } else if (n == 1) {
                    std::string dst;
                    for (int j = int(rdst.length())-1; j >= 0; j--) {
                        dst.push_back(rdst[j]);
                    }
                    return dst;
                } else {
                    n--;
                    rdst.push_back(c);
                }
            }
            return src;
        }
    
        void addFullPath(const std::string& label) { fFullPaths.push_back(buildPath(label)); }
    
        /**
         * @brief Compute the mapping between full path and short names
         */
        void computeShortNames()
        {
            std::vector<std::string>           uniquePaths;  // all full paths transformed but made unique with a prefix
            std::map<std::string, std::string> unique2full;  // all full paths transformed but made unique with a prefix
            char num_buffer[16];
            int pnum = 0;
            
            for (const auto& s : fFullPaths) {
                // Using snprintf since Teensy does not have the std::to_string function
                snprintf(num_buffer, 16, "%d", pnum++);
                std::string u = "/P" + std::string(num_buffer) + str2ID(remove0x00(s));
                uniquePaths.push_back(u);
                unique2full[u] = s;  // remember the full path associated to a unique path
            }
        
            std::map<std::string, int> uniquePath2level;                // map path to level
            for (const auto& s : uniquePaths) uniquePath2level[s] = 1;   // we init all levels to 1
            bool have_collisions = true;
        
            while (have_collisions) {
                // compute collision list
                std::set<std::string>              collisionSet;
                std::map<std::string, std::string> short2full;
                have_collisions = false;
                for (const auto& it : uniquePath2level) {
                    std::string u = it.first;
                    int n = it.second;
                    std::string shortName = cut(u, n);
                    auto p = short2full.find(shortName);
                    if (p == short2full.end()) {
                        // no collision
                        short2full[shortName] = u;
                    } else {
                        // we have a collision, add the two paths to the collision set
                        have_collisions = true;
                        collisionSet.insert(u);
                        collisionSet.insert(p->second);
                    }
                }
                for (const auto& s : collisionSet) uniquePath2level[s]++;  // increase level of colliding path
            }
        
            for (const auto& it : uniquePath2level) {
                std::string u = it.first;
                int n = it.second;
                std::string shortName = replaceCharList(cut(u, n), {'/'}, '_');
                fFull2Short[unique2full[u]] = shortName;
            }
        }
    
        std::string replaceCharList(const std::string& str, const std::vector<char>& ch1, char ch2)
        {
            auto beg = ch1.begin();
            auto end = ch1.end();
            std::string res = str;
            for (size_t i = 0; i < str.length(); ++i) {
                if (std::find(beg, end, str[i]) != end) res[i] = ch2;
            }
            return res;
        }
     
    public:
    
        PathBuilder() {}
        virtual ~PathBuilder() {}
    
        // Return true for the first level of groups
        bool pushLabel(const std::string& label_aux)
        {
            std::string label = replaceCharList(label_aux, {'/'}, '_');
            fControlsLevel.push_back(label); return fControlsLevel.size() == 1;
        }
    
        // Return true for the last level of groups
        bool popLabel() { fControlsLevel.pop_back(); return fControlsLevel.size() == 0; }
    
        // Return a complete path built from a label
        std::string buildPath(const std::string& label_aux)
        {
            std::string label = replaceCharList(label_aux, {'/'}, '_');
            std::string res = "/";
            for (size_t i = 0; i < fControlsLevel.size(); i++) {
                res = res + fControlsLevel[i] + "/";
            }
            res += label;
            return replaceCharList(res, {' ', '#', '*', ',', '?', '[', ']', '{', '}', '(', ')'}, '_');
        }
    
        // Assuming shortnames have been built, return the shortname from a label
        std::string buildShortname(const std::string& label)
        {
            return (hasShortname()) ? fFull2Short[buildPath(label)] : "";
        }
    
        bool hasShortname() { return fFull2Short.size() > 0; }
    
};

#endif  // __PathBuilder__
/**************************  END  PathBuilder.h **************************/

/*******************************************************************************
 * FUI : used to save and recall the state of the user interface
 * This class provides essentially two new methods saveState() and recallState()
 * used to save on file and recall from file the state of the user interface.
 * The file is human readable and editable
 ******************************************************************************/

class FUI : public UI, public PathBuilder
{

    protected:

        std::map<std::string, FAUSTFLOAT*> fName2Zone;
        std::vector<FAUSTFLOAT*> fButtons;

        // add an element by relating its full name and memory zone
        virtual void addElement(const char* label, FAUSTFLOAT* zone, bool button = false)
        {
            std::string path = buildPath(label);
            fName2Zone[path] = zone;
            if (button) {
                fButtons.push_back(zone);
            }
        }

    
    public:

        FUI() {}
        virtual ~FUI() {}

        // -- Save and recall methods

        // save the zones values and full names
        virtual void saveState(const char* filename)
        {
            std::ofstream file(filename);
            if (file.is_open()) {
                for (const auto& it : fName2Zone) {
                    file << *it.second << ' ' << it.first << std::endl;
                }
                file.close();
            } else {
                std::cerr << "Error opening " << filename << " file\n";
            }
        }

        // recall the zones values and full names
        virtual void recallState(const char* filename)
        {
            std::ifstream file(filename);
            FAUSTFLOAT value;
            std::string path1, path2;
            while (file.good()) {
                file >> value >> path1;
                path2 = "/" + path1;
                if (fName2Zone.count(path1) > 0) {          // Old path system
                    *(fName2Zone[path1]) = value;
                } else if (fName2Zone.count(path2) > 0) {   // New path system with the starting '/'
                    *(fName2Zone[path2]) = value;
                } else if (path1.size() > 0) {
                    std::cerr << "recallState : parameter not found : " << path1 << " with value : " << value << std::endl;
                }
            }
            file.close();
        }

        void setButtons(bool state)
        {
            for (size_t i = 0; i < fButtons.size(); i++) {
                *fButtons[i] = state;
            }
        }

        // -- widget's layouts (just keep track of group labels)

        virtual void openTabBox(const char* label) { pushLabel(label); }
        virtual void openHorizontalBox(const char* label) { pushLabel(label);; }
        virtual void openVerticalBox(const char* label) { pushLabel(label); }
        virtual void closeBox() { popLabel(); };

        // -- active widgets (just add an element)

        virtual void addButton(const char* label, FAUSTFLOAT* zone) { addElement(label, zone, true); }
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) { addElement(label, zone); }
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT, FAUSTFLOAT, FAUSTFLOAT, FAUSTFLOAT)
                                                                    { addElement(label, zone); }
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT, FAUSTFLOAT, FAUSTFLOAT, FAUSTFLOAT)
                                                                    { addElement(label, zone); }
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT, FAUSTFLOAT, FAUSTFLOAT, FAUSTFLOAT)
                                                                    { addElement(label, zone); }

        // -- passive widgets (are ignored)

        virtual void addHorizontalBargraph(const char*, FAUSTFLOAT*, FAUSTFLOAT, FAUSTFLOAT) {}
        virtual void addVerticalBargraph(const char*, FAUSTFLOAT*, FAUSTFLOAT, FAUSTFLOAT) {}
    
        // -- soundfiles
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}

        // -- metadata are not used

        virtual void declare(FAUSTFLOAT*, const char*, const char*) {}

};

#endif

/**************************  END  FUI.h **************************/
/************************** BEGIN ControlSequenceUI.h *******************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 *************************************************************************/


#ifndef CONTROL_SEQUENCE_UI_H
#define CONTROL_SEQUENCE_UI_H

#include <vector>
#include <sstream>
#include <fstream>
#include <assert.h>

/************************** BEGIN MapUI.h ******************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ***********************************************************************/

#ifndef FAUST_MAPUI_H
#define FAUST_MAPUI_H

#include <vector>
#include <map>
#include <string>
#include <stdio.h>


/*******************************************************************************
 * MapUI : Faust User Interface.
 *
 * This class creates:
 * - a map of 'labels' and zones for each UI item.
 * - a map of unique 'shortname' (built so that they never collide) and zones for each UI item
 * - a map of complete hierarchical 'paths' and zones for each UI item
 *
 * Simple 'labels', 'shortname' and complete 'paths' (to fully discriminate between possible same
 * 'labels' at different location in the UI hierachy) can be used to access a given parameter.
 ******************************************************************************/

class FAUST_API MapUI : public UI, public PathBuilder
{
    
    protected:
    
        // Label zone map
        std::map<std::string, FAUSTFLOAT*> fLabelZoneMap;
    
        // Shortname zone map
        std::map<std::string, FAUSTFLOAT*> fShortnameZoneMap;
    
        // Full path map
        std::map<std::string, FAUSTFLOAT*> fPathZoneMap;
    
        void addZoneLabel(const std::string& label, FAUSTFLOAT* zone)
        {
            std::string path = buildPath(label);
            fFullPaths.push_back(path);
            fPathZoneMap[path] = zone;
            fLabelZoneMap[label] = zone;
        }
    
    public:
        
        MapUI() {}
        virtual ~MapUI() {}
        
        // -- widget's layouts
        void openTabBox(const char* label)
        {
            pushLabel(label);
        }
        void openHorizontalBox(const char* label)
        {
            pushLabel(label);
        }
        void openVerticalBox(const char* label)
        {
            pushLabel(label);
        }
        void closeBox()
        {
            if (popLabel()) {
                // Shortnames can be computed when all fullnames are known
                computeShortNames();
                // Fill 'shortname' map
                for (const auto& it : fFullPaths) {
                    fShortnameZoneMap[fFull2Short[it]] = fPathZoneMap[it];
                }
            }
        }
        
        // -- active widgets
        void addButton(const char* label, FAUSTFLOAT* zone)
        {
            addZoneLabel(label, zone);
        }
        void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            addZoneLabel(label, zone);
        }
        void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            addZoneLabel(label, zone);
        }
        void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            addZoneLabel(label, zone);
        }
        void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            addZoneLabel(label, zone);
        }
        
        // -- passive widgets
        void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            addZoneLabel(label, zone);
        }
        void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            addZoneLabel(label, zone);
        }
    
        // -- soundfiles
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}
        
        // -- metadata declarations
        virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {}
    
        //-------------------------------------------------------------------------------
        // Public API
        //-------------------------------------------------------------------------------
    
        /**
         * Set the param value.
         *
         * @param str - the UI parameter label/shortname/path
         * @param value - the UI parameter value
         *
         */
        void setParamValue(const std::string& str, FAUSTFLOAT value)
        {
            const auto fPathZoneMapIter = fPathZoneMap.find(str);
            if (fPathZoneMapIter != fPathZoneMap.end()) {
                *fPathZoneMapIter->second = value;
                return;
            }
            
            const auto fShortnameZoneMapIter = fShortnameZoneMap.find(str);
            if (fShortnameZoneMapIter != fShortnameZoneMap.end()) {
                *fShortnameZoneMapIter->second = value;
                return;
            }
            
            const auto fLabelZoneMapIter = fLabelZoneMap.find(str);
            if (fLabelZoneMapIter != fLabelZoneMap.end()) {
                *fLabelZoneMapIter->second = value;
                return;
            }
            
            fprintf(stderr, "ERROR : setParamValue '%s' not found\n", str.c_str());
        }
        
        /**
         * Return the param value.
         *
         * @param str - the UI parameter label/shortname/path
         *
         * @return the param value.
         */
        FAUSTFLOAT getParamValue(const std::string& str) const
        {
            const auto fPathZoneMapIter = fPathZoneMap.find(str);
            if (fPathZoneMapIter != fPathZoneMap.end()) {
                return *fPathZoneMapIter->second;
            }
            
            const auto fShortnameZoneMapIter = fShortnameZoneMap.find(str);
            if (fShortnameZoneMapIter != fShortnameZoneMap.end()) {
                return *fShortnameZoneMapIter->second;
            }
            
            const auto fLabelZoneMapIter = fLabelZoneMap.find(str);
            if (fLabelZoneMapIter != fLabelZoneMap.end()) {
                return *fLabelZoneMapIter->second;
            }
            
            fprintf(stderr, "ERROR : getParamValue '%s' not found\n", str.c_str());
            return 0;
        }
    
        // map access 
        std::map<std::string, FAUSTFLOAT*>& getFullpathMap() { return fPathZoneMap; }
        std::map<std::string, FAUSTFLOAT*>& getShortnameMap() { return fShortnameZoneMap; }
        std::map<std::string, FAUSTFLOAT*>& getLabelMap() { return fLabelZoneMap; }
            
        /**
         * Return the number of parameters in the UI.
         *
         * @return the number of parameters
         */
        int getParamsCount() const { return int(fPathZoneMap.size()); }
        
        /**
         * Return the param path.
         *
         * @param index - the UI parameter index
         *
         * @return the param path
         */
        std::string getParamAddress(int index) const
        {
            if (index < 0 || index > int(fPathZoneMap.size())) {
                return "";
            } else {
                auto it = fPathZoneMap.begin();
                while (index-- > 0 && it++ != fPathZoneMap.end()) {}
                return it->first;
            }
        }
        
        const char* getParamAddress1(int index) const
        {
            if (index < 0 || index > int(fPathZoneMap.size())) {
                return nullptr;
            } else {
                auto it = fPathZoneMap.begin();
                while (index-- > 0 && it++ != fPathZoneMap.end()) {}
                return it->first.c_str();
            }
        }
    
        /**
         * Return the param shortname.
         *
         * @param index - the UI parameter index
         *
         * @return the param shortname
         */
        std::string getParamShortname(int index) const
        {
            if (index < 0 || index > int(fShortnameZoneMap.size())) {
                return "";
            } else {
                auto it = fShortnameZoneMap.begin();
                while (index-- > 0 && it++ != fShortnameZoneMap.end()) {}
                return it->first;
            }
        }
        
        const char* getParamShortname1(int index) const
        {
            if (index < 0 || index > int(fShortnameZoneMap.size())) {
                return nullptr;
            } else {
                auto it = fShortnameZoneMap.begin();
                while (index-- > 0 && it++ != fShortnameZoneMap.end()) {}
                return it->first.c_str();
            }
        }
    
        /**
         * Return the param label.
         *
         * @param index - the UI parameter index
         *
         * @return the param label
         */
        std::string getParamLabel(int index) const
        {
            if (index < 0 || index > int(fLabelZoneMap.size())) {
                return "";
            } else {
                auto it = fLabelZoneMap.begin();
                while (index-- > 0 && it++ != fLabelZoneMap.end()) {}
                return it->first;
            }
        }
        
        const char* getParamLabel1(int index) const
        {
            if (index < 0 || index > int(fLabelZoneMap.size())) {
                return nullptr;
            } else {
                auto it = fLabelZoneMap.begin();
                while (index-- > 0 && it++ != fLabelZoneMap.end()) {}
                return it->first.c_str();
            }
        }
    
        /**
         * Return the param path.
         *
         * @param zone - the UI parameter memory zone
         *
         * @return the param path
         */
        std::string getParamAddress(FAUSTFLOAT* zone) const
        {
            for (const auto& it : fPathZoneMap) {
                if (it.second == zone) return it.first;
            }
            return "";
        }
    
        /**
         * Return the param memory zone.
         *
         * @param zone - the UI parameter label/shortname/path
         *
         * @return the param path
         */
        FAUSTFLOAT* getParamZone(const std::string& str) const
        {
            const auto fPathZoneMapIter = fPathZoneMap.find(str);
            if (fPathZoneMapIter != fPathZoneMap.end()) {
                return fPathZoneMapIter->second;
            }
            
            const auto fShortnameZoneMapIter = fShortnameZoneMap.find(str);
            if (fShortnameZoneMapIter != fShortnameZoneMap.end()) {
                return fShortnameZoneMapIter->second;
            }
            
            const auto fLabelZoneMapIter = fLabelZoneMap.find(str);
            if (fLabelZoneMapIter != fLabelZoneMap.end()) {
                return fLabelZoneMapIter->second;
            }

            return nullptr;
        }
    
        /**
         * Return the param memory zone.
         *
         * @param zone - the UI parameter index
         *
         * @return the param path
         */
        FAUSTFLOAT* getParamZone(int index) const 
        {
            if (index < 0 || index > int(fPathZoneMap.size())) {
                return nullptr;
            } else {
                auto it = fPathZoneMap.begin();
                while (index-- > 0 && it++ != fPathZoneMap.end()) {}
                return it->second;
            }
        }
    
        static bool endsWith(const std::string& str, const std::string& end)
        {
            size_t l1 = str.length();
            size_t l2 = end.length();
            return (l1 >= l2) && (0 == str.compare(l1 - l2, l2, end));
        }
    
};

#endif // FAUST_MAPUI_H
/**************************  END  MapUI.h **************************/

struct TSMessage {
    uint64_t    fDateSample;
    std::string fPath;
    FAUSTFLOAT  fValue;
    
    TSMessage(uint64_t ds, const std::string& path, FAUSTFLOAT value)
    :fDateSample(ds), fPath(path), fValue(value)
    {}
};

/*
 Allows to process a sequence of timestamped messages.
 */
class ControlSequenceUI : public MapUI {
    
    protected:
    
        std::vector<TSMessage> fSequence;
        uint64_t               fCurSample;
        int                    fEvent;
    
        void processMessage(const TSMessage& message)
        {
            setParamValue(message.fPath, message.fValue);
        }
    
    public:
    
        ControlSequenceUI(const std::vector<TSMessage>& sequence):fSequence(sequence), fCurSample(0), fEvent(0) {}
        virtual ~ControlSequenceUI() {}
       
        void process(uint64_t start_sample, uint64_t end_sample)
        {
            if (fSequence.size() == 0) return;
            
            // Restart from begining if needed
            if (start_sample < fCurSample) {
                fCurSample = 0;
                fEvent = 0;
            }
            
            // Move until start of range
            while (fEvent < fSequence.size() && fSequence[fEvent].fDateSample < start_sample) {
                fEvent++;
            }
            
            // Process all messages in [start_sample, end_sample] range
            while (fEvent < fSequence.size() && fSequence[fEvent].fDateSample <= end_sample) {
                processMessage(fSequence[fEvent]);
                fEvent++;
            }
            
            // Keep last date
            fCurSample = fSequence[fEvent].fDateSample;
        }
    
        void display()
        {
            for (const auto& it : fSequence) {
                std::cout << "fDateSample " << it.fDateSample << " fPath " << it.fPath << " fValue " << it.fValue << std::endl;
            }
        }
    
        void getRange(uint64_t& start_sample, uint64_t& end_sample)
        {
            if (fSequence.size() > 0) {
                start_sample = fSequence[0].fDateSample;
                end_sample = fSequence[fSequence.size()-1].fDateSample;
            } else {
                start_sample = end_sample = 0;
            }
        }
        
};

/*
 Read a sequence of messages of type:
 
 e45377aa.206162f1 /karplus/gate f 1.000000
 e45377aa.360afa2e /karplus/gate f 0.000000
 e45377ab.0a40639d /karplus/params/freq f 267.850006
 e45377ab.1f47f993 /karplus/params/freq f 286.850006
 e45377ab.29c88616 /karplus/params/freq f 296.350006
 e45377ab.34370cdc /karplus/params/freq f 305.850006
 
 where a e451f81c.2be22e5d date is a timestamp in sec.frac format coded in hexadecimal.
 
 To be used with ControlSequenceUI class.
 */

struct OSCSequenceReader {
    
    static uint64_t readDate(const std::string& date, int sample_rate)
    {
        std::stringstream tokenizer(date);
        
        std::string sec, frac;
        getline(tokenizer, sec, '.');
        getline(tokenizer, frac, '.');
        
        std::istringstream sec_reader("0x" + sec);
        std::istringstream frac_reader("0x" + frac);
        uint32_t sec_t, frac_t;
        sec_reader >> std::hex >> sec_t;
        frac_reader >> std::hex >> frac_t;
        
        return uint64_t(sample_rate * (double(sec_t) + (double(frac_t)/std::pow(2,32))));
    }
    
    /**
     * Read the file containing timestamped messages.
     *
     * @param filename - the filename path
     * @param sample_rate - the sampling rate, needed to convert dates in seconds into dates in frames
     *
     * @return a vector of timestamped messages [path, value]
     */
    static std::vector<TSMessage> read(const std::string& filename, int sample_rate)
    {
        std::vector<TSMessage> res;
        std::ifstream in_file(filename);
        bool first_date = true;
        uint64_t first_date_sample;
        
        if (in_file.is_open()) {
            std::string line;
            while (getline(in_file, line)) {
                std::istringstream line_reader(line);
                std::string date, path, type;
                uint32_t sec, frac;
                
                line_reader >> date;
                line_reader >> path;
                line_reader >> type;
                
                // Read date, relative to first one
                uint64_t date_sample = readDate(date, sample_rate);
                if (first_date) {
                    first_date_sample = date_sample;
                    first_date = false;
                }
                date_sample -= first_date_sample;
                
                if (type == "i") {
                    int value_int;
                    line_reader >> value_int;
                    res.push_back(TSMessage(date_sample, path, FAUSTFLOAT(value_int)));
                } else if (type == "f") {
                    FAUSTFLOAT value_float;
                    line_reader >> value_float;
                    res.push_back(TSMessage(date_sample, path, value_float));
                }
            }
            in_file.close();
        }
        
        return res;
    }
};

#endif
/**************************  END ControlSequenceUI.h **************************/
/************************** BEGIN dsp-tools.h *****************************
FAUST Architecture File
Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
---------------------------------------------------------------------
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

EXCEPTION : As a special exception, you may create a larger work
that contains this FAUST architecture section and distribute
that work under terms of your choice, so long as this FAUST
architecture section is not modified.
***************************************************************************/

#ifndef __dsp_tools__
#define __dsp_tools__

#include <assert.h>
#include <string.h>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

class Deinterleaver
{
    
    private:
    
        int fNumFrames;
        int fNumInputs;
        int fNumOutputs;
        
        FAUSTFLOAT* fInput;
        FAUSTFLOAT* fOutputs[256];
        
    public:
        
        Deinterleaver(int numFrames, int numInputs, int numOutputs)
        {
            fNumFrames = numFrames;
            fNumInputs = numInputs;
            fNumOutputs = std::max<int>(numInputs, numOutputs);
            
            // allocate interleaved input channel
            fInput = new FAUSTFLOAT[fNumFrames * fNumInputs];
            
            // allocate separate output channels
            for (int i = 0; i < fNumOutputs; i++) {
                fOutputs[i] = new FAUSTFLOAT[fNumFrames];
            }
        }
        
        ~Deinterleaver()
        {
            // free interleaved input channel
            delete [] fInput;
            
            // free separate output channels
            for (int i = 0; i < fNumOutputs; i++) {
                delete [] fOutputs[i];
            }
        }
        
        FAUSTFLOAT* input() { return fInput; }
        
        FAUSTFLOAT** outputs() { return fOutputs; }
        
        void deinterleave()
        {
            for (int s = 0; s < fNumFrames; s++) {
                for (int c = 0; c < fNumInputs; c++) {
                    fOutputs[c][s] = fInput[c + s * fNumInputs];
                }
            }
        }
};

class Interleaver
{
    
    private:
        
        int fNumFrames;
        int fNumInputs;
        int fNumOutputs;
    
        FAUSTFLOAT* fInputs[256];
        FAUSTFLOAT* fOutput;
        
    public:
        
        Interleaver(int numFrames, int numInputs, int numOutputs)
        {
            fNumFrames = numFrames;
            fNumInputs 	= std::max(numInputs, numOutputs);
            fNumOutputs = numOutputs;
            
            // allocate separate input channels
            for (int i = 0; i < fNumInputs; i++) {
                fInputs[i] = new FAUSTFLOAT[fNumFrames];
            }
            
            // allocate interleaved output channel
            fOutput = new FAUSTFLOAT[fNumFrames * fNumOutputs];
        }
        
        ~Interleaver()
        {
            // free separate input channels
            for (int i = 0; i < fNumInputs; i++) {
                delete [] fInputs[i];
            }
            
            // free interleaved output channel
            delete [] fOutput;
        }
        
        FAUSTFLOAT** inputs() { return fInputs; }
        
        FAUSTFLOAT* output() { return fOutput; }
        
        void interleave()
        {
            for (int s = 0; s < fNumFrames; s++) {
                for (int c = 0; c < fNumOutputs; c++) {
                    fOutput[c + s * fNumOutputs] = fInputs[c][s];
                }
            }
        }
};

//=============================================================================
// An AudioChannels is a group of non-interleaved buffers that knows how to read
// from or write to an interleaved buffer. The interleaved buffer may have a
// different number of channels than the AudioChannels internal channels.
//=============================================================================

class AudioChannels
{
    
    protected:
        
        const unsigned int fNumFrames;
        const unsigned int fNumChannels;
        FAUSTFLOAT** fChannels;
        
    public:
        
        AudioChannels(int nframes, int nchannels) : fNumFrames(nframes), fNumChannels(nchannels)
        {
            fChannels = new FAUSTFLOAT*[nchannels];
            
            // allocate audio channels
            for (unsigned int i = 0; i < fNumChannels; i++) {
                fChannels[i] = new FAUSTFLOAT[fNumFrames];
                memset(fChannels[i], 0, sizeof(FAUSTFLOAT) * fNumFrames);
            }
        }
        
        virtual ~AudioChannels()
        {
            // free separate input channels
            for (int i = 0; i < fNumChannels; i++) {
                delete[] fChannels[i];
            }
            delete[] fChannels;
        }
        
        //---------------------------------------------------------------------------------------
        // interleavedRead: read, from the interleaved buffer <inbuffer>, <length> frames on
        // <inchannels> channels. The samples are written to the <fNumChannels> internal
        // <fChannels>.
        void interleavedRead(float* inbuffer, unsigned int length, unsigned int inchannels)
        {
            assert(length <= fNumFrames);
            unsigned int C = std::min<unsigned int>(inchannels, fNumChannels);
            unsigned int F = std::min<unsigned int>(length, fNumFrames);
            
            for (unsigned int f = 0; f < F; f++) {
                unsigned int p = f * inchannels;
                for (unsigned int c = 0; c < C; c++) {
                    fChannels[c][f] = inbuffer[p++];
                }
                for (unsigned int c = C; c < fNumChannels; c++) {
                    fChannels[c][f] = 0;
                }
            }
        }
        
        //----------------------------------------------------------------------------------------
        // interleavedWrite: write to the interleaved buffer <inbuffer>, <length> frames on
        // <outchannels> channels. The samples are read from <fNumChannels> internal
        // <fChannels>.
        void interleavedWrite(float* outbuffer, unsigned int length, unsigned int outchannels)
        {
            assert(length <= fNumFrames);
            unsigned int C = std::min<unsigned int>(outchannels, fNumChannels);
            unsigned int F = std::min<unsigned int>(length, fNumFrames);
            
            for (unsigned int f = 0; f < F; f++) {
                unsigned int p = f * outchannels;
                for (unsigned int c = 0; c < C; c++) {
                    outbuffer[p++] = fChannels[c][f];
                }
                for (unsigned int c = C; c < outchannels; c++) {
                    outbuffer[p++] = 0;
                }
            }
        }
        
        //----------------------------------------------------------------------------------------
        // buffers: the internal buffers ready to use in the compute() method of a Faust dsp
        
        FAUSTFLOAT** buffers() { return fChannels; }
};

// Default DSP
struct default_dsp : public dsp {
    
    int getNumInputs() { return 1; }
    
    int getNumOutputs() { return 1; }
    
    void buildUserInterface(UI* ui_interface) {}
    
    int getSampleRate() { return 44100; }
    
    void init(int sample_rate) {}
    
    void instanceInit(int sample_rate) {}
    
    void instanceConstants(int sample_rate) {}
    
    void instanceResetUserInterface() {}
    
    void instanceClear() {}
    
    default_dsp* clone() { return new default_dsp(); }
    
    void metadata(Meta* m) {}
    
    void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs)
    {}
    
};

#endif
/************************** END dsp-tools.h **************************/
/************************** BEGIN misc.h *******************************
FAUST Architecture File
Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
---------------------------------------------------------------------
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

EXCEPTION : As a special exception, you may create a larger work
that contains this FAUST architecture section and distribute
that work under terms of your choice, so long as this FAUST
architecture section is not modified.
***************************************************************************/

#ifndef __misc__
#define __misc__

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <map>
#include <string.h>
#include <string>

/************************** BEGIN meta.h *******************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __meta__
#define __meta__


/**
 The base class of Meta handler to be used in dsp::metadata(Meta* m) method to retrieve (key, value) metadata.
 */
struct FAUST_API Meta {
    virtual ~Meta() {}
    virtual void declare(const char* key, const char* value) = 0;
};

#endif
/**************************  END  meta.h **************************/

struct MY_Meta : Meta, std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key] = value; }
};

static int lsr(int x, int n) { return int(((unsigned int)x) >> n); }

static int int2pow2(int x) { int r = 0; while ((1<<r) < x) r++; return r; }

static long lopt(char* argv[], const char* name, long def)
{
    for (int i = 0; argv[i]; i++) {
        if (!strcmp(argv[i], name) && argv[i + 1]) {
            return std::strtol(argv[i + 1], nullptr, 10);
        }
    }
    return def;
}

static long lopt1(int argc, char* argv[], const char* longname, const char* shortname, long def)
{
    for (int i = 2; i < argc; i++) {
        if ((strcmp(argv[i - 1], shortname) == 0 || strcmp(argv[i - 1], longname) == 0) && argv[i]) {
            return std::strtol(argv[i], nullptr, 10);
        }
    }
    return def;
}

static const char* lopts(char* argv[], const char* name, const char* def)
{
    for (int i = 0; argv[i]; i++) {
        if (!strcmp(argv[i], name) && argv[i + 1]) {
            return argv[i + 1];
        }
    }
    return def;
}

static const char* lopts1(int argc, char* argv[], const char* longname, const char* shortname, const char* def)
{
    for (int i = 2; i < argc; i++) {
        if ((strcmp(argv[i - 1], shortname) == 0 || strcmp(argv[i - 1], longname) == 0) && argv[i]) {
            return argv[i];
        }
    }
    return def;
}

static bool isopt(char* argv[], const char* name)
{
    for (int i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return true;
    return false;
}

static std::string pathToContent(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ifstream::binary);
    return (!file) ? "" : std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

#endif

/**************************  END  misc.h **************************/

/******************************************************************************
 *******************************************************************************
 
 VECTOR INTRINSICS
 
 *******************************************************************************
 *******************************************************************************/


/********************END ARCHITECTURE SECTION (part 1/2)****************/

/**************************BEGIN USER SECTION **************************/

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

const static float fmydspSIG0Wave0[2001] = {249.98708f,249.98685f,249.98663f,249.9864f,249.98618f,249.98595f,249.9857f,249.98546f,249.98521f,249.98497f,249.98471f,249.98445f,249.98419f,249.98393f,249.98366f,249.98338f,249.98311f,249.98282f,249.98253f,249.98224f,249.98193f,249.98163f,249.98132f,249.981f,249.98068f,249.98036f,249.98003f,249.97969f,249.97935f,249.979f,249.97865f,249.97829f,249.97792f,249.97755f,249.97717f,249.97679f,249.9764f,249.976f,249.9756f,249.97519f,249.97476f,249.97435f,249.9739f,249.97348f,249.97302f,249.97256f,249.9721f,249.97163f,249.97116f,249.97067f,249.97018f,249.96968f,249.96918f,249.96866f,249.96812f,249.96759f,249.96704f,249.96649f,249.96593f,249.96535f,249.96477f,249.96417f,249.96358f,249.96297f,249.96234f,249.96172f,249.96106f,249.9604f,249.95975f,249.95906f,249.95837f,249.95769f,249.95697f,249.95625f,249.9555f,249.95476f,249.95401f,249.95323f,249.95244f,249.95164f,249.95084f,249.95001f,249.94917f,249.94832f,249.94745f,249.94656f,249.94566f,249.94476f,249.94383f,249.94289f,249.94193f,249.94096f,249.93997f,249.93896f,249.93794f,249.93689f,249.93584f,249.93475f,249.93367f,249.93256f,249.93143f,249.93028f,249.92911f,249.92792f,249.92671f,249.92549f,249.92424f,249.92297f,249.92168f,249.92036f,249.91904f,249.91768f,249.9163f,249.9149f,249.91348f,249.91203f,249.91055f,249.90906f,249.90755f,249.90599f,249.90442f,249.90283f,249.9012f,249.89955f,249.89787f,249.89616f,249.89444f,249.89267f,249.89088f,249.88905f,249.8872f,249.88533f,249.8834f,249.88147f,249.87949f,249.87749f,249.87544f,249.87337f,249.87126f,249.86911f,249.86693f,249.86472f,249.86246f,249.86017f,249.85785f,249.85548f,249.85307f,249.85063f,249.84814f,249.84563f,249.84305f,249.84045f,249.8378f,249.8351f,249.83237f,249.82957f,249.82675f,249.82388f,249.82095f,249.818f,249.81497f,249.8119f,249.80878f,249.80562f,249.8024f,249.79912f,249.79579f,249.79242f,249.78899f,249.78549f,249.78195f,249.77834f,249.77467f,249.77095f,249.76717f,249.76332f,249.75941f,249.75545f,249.75142f,249.74731f,249.74315f,249.73892f,249.73462f,249.73026f,249.72581f,249.7213f,249.71672f,249.71205f,249.70732f,249.70251f,249.69763f,249.69267f,249.68762f,249.6825f,249.67729f,249.672f,249.66663f,249.66116f,249.65562f,249.65f,249.64426f,249.63844f,249.63254f,249.62654f,249.62044f,249.61424f,249.60796f,249.60156f,249.59506f,249.58847f,249.58177f,249.57497f,249.56805f,249.56104f,249.55391f,249.54666f,249.5393f,249.53183f,249.52425f,249.51654f,249.5087f,249.50075f,249.49268f,249.48448f,249.47615f,249.4677f,249.4591f,249.45038f,249.44153f,249.43253f,249.4234f,249.41412f,249.40471f,249.39514f,249.38544f,249.37556f,249.36555f,249.3554f,249.34508f,249.3346f,249.32396f,249.31317f,249.3022f,249.29108f,249.27977f,249.26831f,249.25667f,249.24484f,249.23285f,249.22067f,249.2083f,249.19576f,249.18301f,249.17009f,249.15697f,249.14366f,249.13014f,249.11642f,249.10251f,249.08838f,249.07405f,249.0595f,249.04474f,249.02975f,249.01456f,248.99913f,248.98349f,248.9676f,248.9515f,248.93517f,248.91858f,248.90175f,248.88469f,248.86737f,248.84981f,248.832f,248.81392f,248.7956f,248.777f,248.75813f,248.73901f,248.7196f,248.69994f,248.67998f,248.65974f,248.63922f,248.61841f,248.5973f,248.57591f,248.55421f,248.53221f,248.5099f,248.48729f,248.46436f,248.44112f,248.41756f,248.39366f,248.36945f,248.3449f,248.32002f,248.2948f,248.26923f,248.24332f,248.21707f,248.19046f,248.16348f,248.13615f,248.10846f,248.0804f,248.05196f,248.02315f,247.99396f,247.96439f,247.93442f,247.90407f,247.8733f,247.84216f,247.81061f,247.77864f,247.74626f,247.71347f,247.68027f,247.64662f,247.61256f,247.57806f,247.54314f,247.50777f,247.47195f,247.4357f,247.39899f,247.36182f,247.32417f,247.28609f,247.24751f,247.20848f,247.16898f,247.12898f,247.0885f,247.04753f,247.00607f,246.96411f,246.92166f,246.8787f,246.83522f,246.79123f,246.74672f,246.7017f,246.65614f,246.61006f,246.56345f,246.5163f,246.46861f,246.42036f,246.37158f,246.32224f,246.27232f,246.22186f,246.17084f,246.11925f,246.06708f,246.01433f,245.961f,245.90707f,245.85257f,245.79747f,245.74178f,245.68549f,245.62859f,245.57109f,245.51297f,245.45424f,245.39488f,245.33492f,245.2743f,245.21309f,245.15121f,245.08871f,245.02557f,244.96179f,244.89737f,244.83229f,244.76656f,244.70015f,244.6331f,244.56538f,244.497f,244.42795f,244.35822f,244.28781f,244.21672f,244.14496f,244.07251f,243.99936f,243.92552f,243.851f,243.77577f,243.69984f,243.6232f,243.54587f,243.46782f,243.38905f,243.30959f,243.2294f,243.14848f,243.06685f,242.9845f,242.90141f,242.8176f,242.73305f,242.64778f,242.56175f,242.475f,242.38751f,242.29927f,242.21028f,242.12056f,242.03008f,241.93884f,241.84686f,241.75412f,241.66063f,241.56638f,241.47136f,241.37558f,241.27904f,241.18173f,241.08366f,240.98482f,240.88521f,240.78482f,240.68367f,240.58174f,240.47903f,240.37555f,240.27129f,240.16624f,240.06042f,239.95383f,239.84645f,239.73828f,239.62933f,239.51959f,239.40907f,239.29776f,239.18565f,239.07277f,238.95909f,238.84462f,238.72937f,238.61333f,238.49648f,238.37885f,238.26042f,238.1412f,238.0212f,237.90039f,237.77878f,237.65639f,237.5332f,237.40921f,237.28444f,237.15886f,237.03249f,236.90533f,236.77737f,236.6486f,236.51906f,236.38872f,236.25757f,236.12564f,235.9929f,235.85938f,235.72507f,235.58995f,235.45404f,235.31734f,235.17986f,235.04156f,234.90248f,234.76262f,234.62196f,234.48051f,234.33827f,234.19525f,234.05142f,233.90683f,233.76143f,233.61525f,233.46829f,233.32054f,233.17201f,233.0227f,232.8726f,232.72173f,232.57007f,232.41762f,232.2644f,232.11041f,231.95564f,231.8001f,231.64377f,231.48668f,231.3288f,231.17017f,231.01076f,230.85057f,230.68962f,230.52791f,230.36543f,230.2022f,230.03818f,229.87341f,229.70789f,229.5416f,229.37454f,229.20674f,229.03818f,228.86887f,228.6988f,228.52798f,228.35641f,228.18411f,228.01105f,227.83723f,227.66269f,227.4874f,227.31136f,227.13458f,226.95708f,226.77882f,226.59984f,226.42012f,226.23967f,226.05849f,225.87659f,225.69395f,225.51059f,225.3265f,225.1417f,224.95618f,224.76993f,224.58296f,224.39528f,224.20688f,224.01778f,223.82796f,223.63744f,223.4462f,223.25426f,223.06161f,222.86827f,222.67422f,222.47948f,222.28403f,222.08789f,221.89107f,221.69354f,221.49533f,221.29642f,221.09683f,220.89656f,220.6956f,220.49396f,220.29164f,220.08865f,219.88498f,219.68063f,219.4756f,219.26991f,219.06355f,218.85652f,218.64883f,218.44048f,218.23146f,218.02179f,217.81145f,217.60045f,217.38881f,217.1765f,216.96355f,216.74995f,216.5357f,216.32082f,216.10527f,215.88908f,215.67227f,215.4548f,215.23671f,215.01797f,214.79861f,214.57863f,214.358f,214.13675f,213.91487f,213.69237f,213.46924f,213.24548f,213.02112f,212.79614f,212.57054f,212.34433f,212.11751f,211.89008f,211.66205f,211.4334f,211.20415f,210.97429f,210.74384f,210.51279f,210.28113f,210.04889f,209.81604f,209.58261f,209.34859f,209.11397f,208.87877f,208.64299f,208.40663f,208.16968f,207.93214f,207.69405f,207.45535f,207.2161f,206.97627f,206.73587f,206.49489f,206.25336f,206.01125f,205.76859f,205.52534f,205.28156f,205.0372f,204.7923f,204.54683f,204.30081f,204.05424f,203.80711f,203.55945f,203.31123f,203.06247f,202.81316f,202.56331f,202.31291f,202.06198f,201.81052f,201.55852f,201.30598f,201.05292f,200.79932f,200.54518f,200.29053f,200.03535f,199.77965f,199.52342f,199.26668f,199.0094f,198.75162f,198.49332f,198.23451f,197.97519f,197.71535f,197.455f,197.19415f,196.93279f,196.67093f,196.40855f,196.14569f,195.88232f,195.61845f,195.3541f,195.08923f,194.82388f,194.55804f,194.2917f,194.02489f,193.75757f,193.48978f,193.2215f,192.95274f,192.6835f,192.41379f,192.14359f,191.87291f,191.60176f,191.33015f,191.05806f,190.78549f,190.51247f,190.23897f,189.96501f,189.69058f,189.4157f,189.14035f,188.86455f,188.58827f,188.31155f,188.03438f,187.75676f,187.47868f,187.20015f,186.92117f,186.64175f,186.3619f,186.08157f,185.80083f,185.51964f,185.238f,184.95593f,184.67343f,184.39049f,184.10712f,183.8233f,183.53908f,183.25441f,182.96933f,182.68382f,182.39787f,182.11153f,181.82474f,181.53755f,181.24994f,180.9619f,180.67346f,180.3846f,180.09534f,179.80566f,179.51558f,179.22508f,178.93419f,178.6429f,178.3512f,178.0591f,177.7666f,177.47371f,177.18042f,176.88673f,176.59267f,176.2982f,176.00336f,175.70811f,175.4125f,175.1165f,174.82011f,174.52335f,174.22621f,173.9287f,173.6308f,173.33253f,173.0339f,172.7349f,172.43553f,172.13579f,171.8357f,171.53523f,171.23442f,170.93324f,170.63171f,170.32983f,170.0276f,169.72502f,169.42209f,169.11882f,168.8152f,168.51125f,168.20694f,167.90231f,167.59735f,167.29205f,166.98642f,166.68047f,166.37419f,166.0676f,165.76067f,165.45343f,165.14589f,164.83803f,164.52985f,164.22137f,163.9126f,163.60352f,163.29414f,162.98447f,162.6745f,162.36426f,162.05371f,161.7429f,161.43181f,161.12044f,160.80879f,160.49689f,160.18471f,159.87227f,159.55959f,159.24664f,158.93344f,158.62001f,158.30634f,157.99243f,157.67828f,157.36392f,157.04933f,156.73453f,156.41951f,156.10428f,155.78885f,155.47324f,155.15742f,154.84143f,154.52525f,154.20891f,153.89241f,153.57573f,153.25891f,152.94196f,152.62485f,152.3076f,151.99025f,151.67278f,151.3552f,151.0375f,150.71973f,150.40187f,150.08394f,149.76595f,149.44789f,149.1298f,148.81168f,148.49352f,148.17535f,147.85718f,147.53902f,147.22087f,146.90276f,146.58469f,146.26668f,145.94873f,145.63087f,145.31313f,144.99547f,144.67795f,144.36057f,144.04333f,143.72629f,143.40942f,143.09276f,142.77632f,142.46013f,142.14418f,141.82852f,141.51315f,141.19809f,140.88336f,140.569f,140.255f,139.94139f,139.62822f,139.31548f,139.00319f,138.69139f,138.3801f,138.06934f,137.75914f,137.44952f,137.1405f,136.83212f,136.52438f,136.21735f,135.91101f,135.60541f,135.30058f,134.99654f,134.69333f,134.39095f,134.08946f,133.78886f,133.48921f,133.19052f,132.89282f,132.59615f,132.30052f,132.00598f,131.71254f,131.42026f,131.12914f,130.8392f,130.5505f,130.26306f,129.9769f,129.69206f,129.40857f,129.12643f,128.8457f,128.56639f,128.28853f,128.01216f,127.73728f,127.463936f,127.19214f,126.92192f,126.653305f,126.38631f,126.12096f,125.85727f,125.595276f,125.33498f,125.07641f,124.81957f,124.5645f,124.3112f,124.05969f,123.80999f,123.562096f,123.31603f,123.071815f,122.82944f,122.58892f,122.35027f,122.1135f,121.87861f,121.64561f,121.41449f,121.18527f,120.95795f,120.73252f,120.509f,120.28738f,120.06764f,119.849815f,119.63388f,119.41983f,119.20767f,118.9974f,118.788994f,118.58246f,118.37779f,118.17497f,117.97401f,117.77487f,117.57757f,117.38209f,117.18841f,116.99652f,116.80643f,116.6181f,116.43154f,116.24672f,116.063644f,115.88228f,115.70263f,115.524666f,115.34839f,115.173775f,115.0008f,114.829475f,114.65976f,114.491646f,114.32513f,114.16018f,113.99679f,113.83494f,113.674614f,113.5158f,113.35848f,113.20264f,113.04825f,112.89532f,112.743805f,112.59371f,112.44501f,112.29769f,112.15174f,112.00714f,111.86387f,111.72192f,111.58127f,111.44191f,111.30382f,111.166985f,111.03139f,110.89702f,110.76386f,110.631905f,110.50112f,110.371506f,110.24305f,110.11572f,109.989525f,109.86443f,109.74044f,109.61753f,109.49569f,109.3749f,109.25515f,109.13644f,109.01873f,108.90203f,108.78632f,108.67159f,108.55783f,108.445015f,108.333145f,108.222206f,108.11218f,108.00306f,107.89484f,107.7875f,107.68103f,107.575424f,107.47067f,107.36675f,107.26367f,107.1614f,107.059944f,106.95928f,106.85941f,106.760315f,106.661995f,106.56443f,106.46762f,106.371544f,106.27621f,106.18159f,106.087685f,105.99449f,105.901985f,105.81017f,105.71905f,105.628586f,105.53879f,105.449646f,105.36115f,105.2733f,105.18607f,105.09948f,105.013504f,104.92813f,104.84337f,104.75919f,104.67561f,104.59261f,104.51019f,104.42834f,104.34704f,104.266304f,104.18611f,104.10646f,104.02735f,103.94877f,103.87071f,103.793175f,103.71615f,103.63963f,103.563614f,103.48809f,103.413055f,103.33851f,103.26444f,103.19084f,103.117714f,103.04505f,102.97285f,102.90109f,102.82979f,102.75893f,102.68851f,102.618515f,102.54896f,102.47982f,102.41111f,102.34281f,102.274925f,102.20744f,102.140366f,102.07369f,102.00741f,101.94151f,101.876f,101.810875f,101.74613f,101.681755f,101.61776f,101.55412f,101.49085f,101.42794f,101.36538f,101.30318f,101.241325f,101.17981f,101.118645f,101.057816f,100.99732f,100.93716f,100.87733f,100.81782f,100.75863f,100.69976f,100.64121f,100.58298f,100.52505f,100.46743f,100.41011f,100.353096f,100.29638f,100.23996f,100.18383f,100.12799f,100.07244f,100.01717f,99.9622f,99.90749f,99.853065f,99.79891f,99.74503f,99.69142f,99.63808f,99.585f,99.53218f,99.47962f,99.42732f,99.375275f,99.32349f,99.27195f,99.22066f,99.16961f,99.118805f,99.068245f,99.01793f,98.96785f,98.91801f,98.86839f,98.819016f,98.76986f,98.72094f,98.67225f,98.62377f,98.57552f,98.5275f,98.47968f,98.43209f,98.38471f,98.33755f,98.290596f,98.24385f,98.19731f,98.15098f,98.10485f,98.05893f,98.01321f,97.96768f,97.92236f,97.87723f,97.8323f,97.78756f,97.74301f,97.698654f,97.65448f,97.6105f,97.5667f,97.52309f,97.47966f,97.43641f,97.39334f,97.35046f,97.30775f,97.265205f,97.22285f,97.18066f,97.13864f,97.096794f,97.05512f,97.01361f,96.972275f,96.9311f,96.89008f,96.849236f,96.80855f,96.76803f,96.72766f,96.687454f,96.6474f,96.607506f,96.56777f,96.52818f,96.488754f,96.44947f,96.41034f,96.37136f,96.33253f,96.29384f,96.2553f,96.21691f,96.17866f,96.14056f,96.10259f,96.064766f,96.027084f,95.98954f,95.95214f,95.91487f,95.87774f,95.840744f,95.80389f,95.76716f,95.73056f,95.6941f,95.65777f,95.62157f,95.585495f,95.54955f,95.51373f,95.47804f,95.44248f,95.40704f,95.37173f,95.33653f,95.30146f,95.26652f,95.23169f,95.19698f,95.1624f,95.12793f,95.093575f,95.05934f,95.02522f,94.99123f,94.95734f,94.92356f,94.8899f,94.85635f,94.82292f,94.7896f,94.756386f,94.72328f,94.690285f,94.6574f,94.62462f,94.59195f,94.55939f,94.526924f,94.494576f,94.462326f,94.430176f,94.39814f,94.366196f,94.33436f,94.30263f,94.27099f,94.239456f,94.208015f,94.17668f,94.14545f,94.1143f,94.08326f,94.052315f,94.02146f,93.99071f,93.96005f,93.92948f,93.89901f,93.86864f,93.83835f,93.80816f,93.77805f,93.74805f,93.718124f,93.68829f,93.658554f,93.62891f,93.59934f,93.56986f,93.54048f,93.51118f,93.481964f,93.45284f,93.4238f,93.394844f,93.36597f,93.33718f,93.30848f,93.27986f,93.25132f,93.22286f,93.19449f,93.1662f,93.13799f,93.109856f,93.08181f,93.05384f,93.02595f,92.99814f,92.9704f,92.94274f,92.91517f,92.887665f,92.860245f,92.83289f,92.80562f,92.77843f,92.751305f,92.72426f,92.69728f,92.67039f,92.64356f,92.616806f,92.590126f,92.56352f,92.53699f,92.51053f,92.48413f,92.45782f,92.431564f,92.40539f,92.37927f,92.35323f,92.32726f,92.30136f,92.27553f,92.24976f,92.22406f,92.19843f,92.17287f,92.14737f,92.12194f,92.09657f,92.07127f,92.04604f,92.020874f,91.99577f,91.97073f,91.94576f,91.920845f,91.896f,91.871216f,91.8465f,91.82184f,91.79724f,91.772705f,91.74823f,91.72382f,91.69947f,91.675186f,91.650955f,91.626785f,91.60268f,91.57863f,91.554634f,91.53071f,91.506836f,91.483025f,91.45927f,91.43557f,91.41193f,91.38835f,91.36482f,91.341354f,91.31795f,91.294586f,91.27129f,91.24805f,91.22486f,91.20173f,91.17865f,91.15563f,91.13266f,91.10975f,91.08689f,91.06409f,91.041336f,91.01864f,90.995995f,90.9734f,90.95086f,90.928375f,90.90594f,90.88355f,90.86122f,90.83894f,90.81672f,90.79454f,90.772415f,90.750336f,90.72831f,90.70634f,90.68441f,90.66254f,90.64071f,90.61893f,90.5972f,90.57552f,90.553894f,90.53231f,90.51077f,90.48929f,90.46785f,90.446465f,90.42512f,90.403824f,90.382576f,90.361374f,90.34022f,90.31911f,90.29804f,90.27702f,90.25606f,90.23513f,90.21425f,90.19341f,90.17262f,90.15188f,90.13118f,90.11052f,90.08991f,90.06934f,90.04882f,90.02834f,90.007904f,89.98751f,89.96716f,89.94685f,89.92659f,89.90637f,89.88619f,89.86606f,89.84596f,89.82591f,89.8059f,89.785934f,89.76601f,89.74612f,89.72627f,89.70647f,89.68671f,89.666985f,89.6473f,89.62766f,89.60806f,89.5885f,89.56898f,89.5495f,89.53006f,89.51066f,89.491295f,89.47197f,89.45268f,89.43343f,89.41423f,89.39506f,89.37593f,89.356834f,89.33778f,89.31876f,89.29979f,89.280846f,89.26194f,89.24307f,89.22424f,89.20545f,89.1867f,89.16798f,89.1493f,89.13065f,89.112045f,89.09347f,89.074936f,89.056435f,89.037964f,89.01954f,89.001144f,88.98279f,88.96446f,88.946175f,88.927925f,88.909706f,88.89152f,88.87337f,88.855255f,88.83717f,88.81913f,88.80112f,88.78314f,88.7652f,88.74729f,88.72941f,88.71157f,88.693756f,88.67598f,88.65824f,88.640526f,88.62285f,88.60521f,88.58759f,88.570015f,88.55247f,88.53496f,88.51747f,88.50002f,88.482605f,88.46522f,88.44786f,88.43054f,88.413246f,88.39599f,88.37876f,88.361565f,88.3444f,88.32726f,88.31016f,88.29308f,88.27604f,88.259026f,88.24204f,88.22509f,88.20817f,88.19128f,88.174416f,88.157585f,88.14078f,88.12401f,88.10727f,88.09055f,88.07387f,88.05721f,88.04059f,88.023994f,88.00742f,87.99088f,87.97438f,87.95789f,87.941444f,87.92502f,87.90862f,87.89226f,87.875916f,87.85961f,87.84332f,87.82707f,87.810844f,87.79465f,87.77847f,87.76233f,87.746216f,87.730125f,87.714066f,87.69803f,87.68202f,87.66604f,87.650085f,87.63416f,87.61826f,87.60239f,87.58654f,87.570724f,87.55493f,87.53916f,87.52342f,87.507706f,87.49202f,87.47636f,87.46072f,87.44511f,87.42952f,87.41396f,87.39843f,87.38292f,87.36744f,87.35198f,87.33655f,87.32114f,87.305756f,87.2904f,87.27507f,87.25976f,87.24448f,87.22922f,87.21399f,87.19878f,87.18359f,87.168434f,87.1533f,87.13819f,87.1231f,87.10804f,87.093f,87.07799f,87.062996f,87.04803f,87.03308f,87.018166f,87.003265f,86.988396f,86.97354f,86.95872f,86.94392f,86.92914f,86.914375f,86.89964f,86.88493f,86.87025f,86.85558f,86.84094f,86.826324f,86.81172f,86.79715f,86.7826f,86.768074f,86.75356f,86.73908f,86.72462f,86.71018f,86.69576f,86.681366f,86.66699f,86.65264f,86.63831f,86.624f,86.60972f,86.59545f,86.58121f,86.566986f,86.55279f,86.53861f,86.52445f,86.510315f,86.4962f,86.48211f,86.46803f,86.45399f,86.43996f,86.42594f,86.41196f,86.39799f,86.38404f,86.37012f,86.35621f,86.34232f,86.32846f,86.31461f,86.30079f,86.28699f,86.2732f,86.25944f,86.2457f,86.23197f,86.21827f,86.20459f,86.190926f,86.177284f,86.16366f,86.150055f,86.136475f,86.12291f,86.10936f,86.09584f,86.08233f,86.06885f,86.05538f,86.04193f,86.0285f,86.01509f,86.0017f,85.988335f,85.97498f,85.96165f,85.94833f,85.93504f,85.92176f,85.90851f,85.89526f,85.88205f,85.86884f,85.85566f,85.8425f,85.82935f,85.81622f,85.803116f,85.79002f,85.776955f,85.7639f,85.75086f,85.73785f,85.724846f,85.71186f,85.6989f,85.68595f,85.67303f,85.66012f,85.647224f,85.634346f,85.62149f,85.60865f,85.59583f,85.58302f,85.57024f,85.55747f,85.544716f,85.53198f,85.519264f,85.50657f,85.49388f,85.48122f,85.46857f,85.45594f,85.44332f,85.430725f,85.418144f,85.40558f,85.393036f,85.38051f,85.36799f,85.3555f,85.34302f,85.33055f,85.31811f,85.30568f,85.29327f,85.28087f,85.268486f,85.25612f,85.243774f,85.231445f,85.21913f,85.20683f,85.19455f,85.18228f,85.17003f,85.15779f,85.14558f,85.13338f,85.121185f,85.10902f,85.09686f,85.084724f,85.0726f,85.06049f,85.0484f,85.03632f,85.02426f,85.012215f,85.00019f,84.988174f,84.97617f,84.964195f,84.952225f,84.94027f,84.92834f,84.91641f,84.90451f,84.89262f,84.88074f,84.86888f,84.85703f,84.8452f,84.83338f,84.82159f,84.8098f,84.79803f,84.78627f,84.77453f,84.7628f,84.75109f,84.73939f,84.72771f,84.71604f,84.704384f,84.69275f,84.68112f,84.66951f,84.65791f,84.64634f,84.634766f,84.623215f,84.61168f,84.60015f,84.588646f,84.57715f,84.56567f,84.5542f,84.54275f,84.53131f,84.51988f,84.50848f,84.49708f,84.485695f,84.47433f,84.46297f,84.45163f,84.4403f,84.428986f,84.41769f,84.4064f,84.39513f,84.383865f,84.37262f,84.36139f,84.350174f,84.33897f,84.327774f,84.3166f,84.305435f,84.29428f,84.28314f,84.27202f,84.26091f,84.24981f,84.23872f,84.22765f,84.2166f,84.20555f,84.19452f,84.1835f,84.17249f,84.1615f,84.15052f,84.13956f,84.1286f,84.11766f,84.106735f,84.09582f,84.084915f,84.07403f,84.06315f,84.052284f,84.04143f,84.030594f,84.01977f,84.00895f,83.99815f,83.987366f,83.976585f,83.96582f,83.95507f,83.944336f,83.93361f,83.92289f,83.91219f,83.901505f,83.89082f,83.88016f,83.86951f,83.858864f,83.848236f,83.83762f,83.82702f,83.81643f,83.80585f,83.79528f,83.78472f,83.77418f,83.76365f,83.75313f,83.74262f,83.732124f,83.72164f,83.711174f,83.70071f,83.69026f,83.679825f,83.6694f,83.65899f,83.64858f,83.63819f,83.627815f,83.61745f,83.60709f,83.59675f,83.58642f,83.576096f,83.56579f,83.55549f,83.545204f,83.53493f,83.524666f,83.51441f,83.50417f,83.49394f,83.48373f,83.47352f,83.463326f,83.45314f,83.44296f,83.4328f,83.42265f,83.41251f,83.40238f,83.392265f,83.38216f,83.37206f,83.36198f,83.351906f,83.34184f,83.33179f,83.32175f,83.311714f,83.3017f,83.29169f,83.28169f,83.271706f,83.26173f,83.25176f,83.241806f,83.231865f,83.22193f,83.212006f,83.202095f,83.19219f,83.182304f,83.172424f,83.16255f,83.152695f,83.142845f,83.13301f,83.123184f,83.113365f,83.10355f,83.09376f,83.08398f,83.074196f,83.06443f,83.05468f,83.04493f,83.035194f,83.025475f,83.015755f,83.00606f,82.99636f,82.98668f,82.977005f,82.96734f,82.95769f,82.948044f,82.93841f,82.92879f,82.919174f,82.90957f,82.89998f,82.890396f,82.88082f,82.871254f,82.8617f,82.85216f,82.84262f,82.8331f,82.823586f,82.81408f,82.80459f,82.795105f,82.78563f,82.77616f,82.7667f,82.757256f,82.74782f,82.738396f,82.72898f,82.71957f,82.710175f,82.70078f,82.69141f,82.68204f,82.672676f,82.66332f,82.653984f,82.64465f,82.63533f,82.626015f,82.616714f,82.607414f,82.59813f,82.58886f,82.57959f,82.570335f,82.56109f,82.55185f,82.54262f,82.5334f,82.524185f,82.514984f,82.5058f,82.49661f,82.487434f,82.47827f,82.46912f,82.45997f,82.45083f,82.4417f,82.43259f,82.42347f,82.41437f,82.40528f,82.396194f,82.38712f,82.37805f,82.368996f,82.35995f,82.35091f,82.34188f,82.332855f,82.323845f,82.31484f,82.30585f,82.29686f,82.28788f};
class mydspSIG0 {
	
  private:
	
	int fmydspSIG0Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG0() {
		return 0;
	}
	int getNumOutputsmydspSIG0() {
		return 1;
	}
	
	void instanceInitmydspSIG0(int sample_rate) {
		fmydspSIG0Wave0_idx = 0;
	}
	
	void fillmydspSIG0(int count, float* table) {
		for (int i1 = 0; i1 < count; i1 = i1 + 1) {
			table[i1] = fmydspSIG0Wave0[fmydspSIG0Wave0_idx];
			fmydspSIG0Wave0_idx = (1 + fmydspSIG0Wave0_idx) % 2001;
		}
	}

};

static mydspSIG0* newmydspSIG0() { return (mydspSIG0*)new mydspSIG0(); }
static void deletemydspSIG0(mydspSIG0* dsp) { delete dsp; }

const static float fmydspSIG1Wave0[2001] = {223.48497f,223.3819f,223.27861f,223.1751f,223.07137f,222.96742f,222.86325f,222.75887f,222.65427f,222.54945f,222.44441f,222.33916f,222.23369f,222.128f,222.0221f,221.91597f,221.80963f,221.70308f,221.59631f,221.48933f,221.38213f,221.2747f,221.16708f,221.05923f,220.95117f,220.8429f,220.7344f,220.6257f,220.51678f,220.40765f,220.29831f,220.18875f,220.07898f,219.969f,219.8588f,219.7484f,219.63777f,219.52693f,219.4159f,219.30464f,219.19316f,219.08148f,218.96959f,218.8575f,218.74518f,218.63266f,218.51993f,218.40698f,218.29384f,218.18047f,218.06691f,217.95312f,217.83913f,217.72493f,217.61053f,217.49591f,217.38109f,217.26607f,217.15083f,217.03539f,216.91972f,216.80386f,216.6878f,216.57153f,216.45505f,216.33836f,216.22148f,216.10439f,215.98708f,215.86957f,215.75186f,215.63394f,215.51582f,215.39749f,215.27896f,215.16023f,215.04129f,214.92215f,214.80281f,214.68326f,214.5635f,214.44356f,214.3234f,214.20303f,214.08247f,213.96172f,213.84074f,213.71959f,213.59822f,213.47665f,213.35489f,213.23291f,213.11075f,212.98837f,212.86581f,212.74304f,212.62007f,212.4969f,212.37354f,212.24997f,212.1262f,212.00224f,211.87808f,211.75372f,211.62918f,211.50443f,211.37947f,211.25432f,211.12898f,211.00343f,210.8777f,210.75177f,210.62564f,210.49931f,210.37279f,210.24606f,210.11916f,209.99205f,209.86475f,209.73726f,209.60956f,209.48167f,209.35359f,209.22533f,209.09686f,208.9682f,208.83936f,208.7103f,208.58107f,208.45163f,208.322f,208.1922f,208.0622f,207.93199f,207.8016f,207.67102f,207.54025f,207.40929f,207.27814f,207.14679f,207.01526f,206.88353f,206.75162f,206.61952f,206.48723f,206.35475f,206.22208f,206.08922f,205.95618f,205.82294f,205.68951f,205.5559f,205.4221f,205.28812f,205.15393f,205.01958f,204.88503f,204.75029f,204.61537f,204.48026f,204.34497f,204.20949f,204.07382f,203.93797f,203.80194f,203.66571f,203.52931f,203.39272f,203.25594f,203.11897f,202.98183f,202.8445f,202.70699f,202.56929f,202.43141f,202.29335f,202.1551f,202.01668f,201.87807f,201.73927f,201.60031f,201.46115f,201.32181f,201.18228f,201.04259f,200.90271f,200.76263f,200.62239f,200.48196f,200.34135f,200.20058f,200.0596f,199.91846f,199.77713f,199.63562f,199.49394f,199.35208f,199.21004f,199.06781f,198.92542f,198.78284f,198.64008f,198.49713f,198.35402f,198.21072f,198.06726f,197.92361f,197.77979f,197.63579f,197.49161f,197.34726f,197.20273f,197.05801f,196.91313f,196.76808f,196.62285f,196.47743f,196.33185f,196.18608f,196.04015f,195.89404f,195.74776f,195.60129f,195.45465f,195.30785f,195.16086f,195.0137f,194.86638f,194.71887f,194.5712f,194.42334f,194.27531f,194.12712f,193.97876f,193.83022f,193.6815f,193.53261f,193.38354f,193.23433f,193.08492f,192.93535f,192.7856f,192.63568f,192.4856f,192.33534f,192.1849f,192.03432f,191.88354f,191.7326f,191.5815f,191.4302f,191.27876f,191.12714f,190.97536f,190.8234f,190.67126f,190.51897f,190.3665f,190.21387f,190.06107f,189.9081f,189.75496f,189.60165f,189.44818f,189.29454f,189.14073f,188.98676f,188.8326f,188.67828f,188.5238f,188.36917f,188.21436f,188.05937f,187.90422f,187.74892f,187.59343f,187.43779f,187.28198f,187.126f,186.96986f,186.81355f,186.65709f,186.50044f,186.34364f,186.18668f,186.02954f,185.87225f,185.71478f,185.55716f,185.39937f,185.24142f,185.0833f,184.92502f,184.76657f,184.60797f,184.4492f,184.29027f,184.13116f,183.97191f,183.81248f,183.6529f,183.49315f,183.33324f,183.17316f,183.01292f,182.85252f,182.69197f,182.53125f,182.37036f,182.20932f,182.04811f,181.88675f,181.72522f,181.56352f,181.40167f,181.23967f,181.0775f,180.91516f,180.75267f,180.59001f,180.4272f,180.26422f,180.10109f,179.9378f,179.77435f,179.61073f,179.44696f,179.28304f,179.11894f,178.9547f,178.79028f,178.62572f,178.46098f,178.29611f,178.13106f,177.96587f,177.8005f,177.63498f,177.4693f,177.30347f,177.13748f,176.97133f,176.80502f,176.63855f,176.47192f,176.30515f,176.13821f,175.97112f,175.80386f,175.63646f,175.46889f,175.30116f,175.13329f,174.96526f,174.79706f,174.62872f,174.4602f,174.29155f,174.12274f,173.95377f,173.78464f,173.61536f,173.4459f,173.27632f,173.10657f,172.93666f,172.7666f,172.59639f,172.42601f,172.2555f,172.08481f,171.91397f,171.74298f,171.57184f,171.40053f,171.22908f,171.05748f,170.88571f,170.71379f,170.54172f,170.36949f,170.19711f,170.02458f,169.8519f,169.67906f,169.50607f,169.33292f,169.15962f,168.98618f,168.81256f,168.63881f,168.46489f,168.29082f,168.11661f,167.94223f,167.76772f,167.59303f,167.4182f,167.24323f,167.06808f,166.8928f,166.71736f,166.54176f,166.36603f,166.19012f,166.01408f,165.83789f,165.66153f,165.48503f,165.30838f,165.13156f,164.9546f,164.7775f,164.60023f,164.42282f,164.24527f,164.06755f,163.88968f,163.71167f,163.5335f,163.35518f,163.17671f,162.9981f,162.81932f,162.6404f,162.46132f,162.2821f,162.10272f,161.9232f,161.74353f,161.5637f,161.38373f,161.2036f,161.02333f,160.8429f,160.66232f,160.4816f,160.30072f,160.1197f,159.93852f,159.7572f,159.57573f,159.3941f,159.21233f,159.0304f,158.84833f,158.6661f,158.48373f,158.30121f,158.11853f,157.93571f,157.75275f,157.56963f,157.38635f,157.20294f,157.01938f,156.83566f,156.6518f,156.46779f,156.28362f,156.0993f,155.91486f,155.73024f,155.54549f,155.36058f,155.17552f,154.99033f,154.80496f,154.61946f,154.43382f,154.24802f,154.06207f,153.87598f,153.68974f,153.50334f,153.3168f,153.13013f,152.94328f,152.7563f,152.56917f,152.3819f,152.19446f,152.00688f,151.81917f,151.63129f,151.44327f,151.2551f,151.06679f,150.87833f,150.68971f,150.50095f,150.31204f,150.12299f,149.93378f,149.74443f,149.55493f,149.36528f,149.17549f,148.98555f,148.79546f,148.60522f,148.41484f,148.2243f,148.03362f,147.84279f,147.65182f,147.4607f,147.26942f,147.078f,146.88644f,146.69473f,146.50287f,146.31087f,146.11871f,145.9264f,145.73395f,145.54135f,145.34862f,145.15572f,144.96268f,144.76949f,144.57616f,144.38268f,144.18904f,143.99527f,143.80135f,143.60727f,143.41306f,143.21869f,143.02417f,142.82951f,142.6347f,142.43976f,142.24464f,142.0494f,141.854f,141.65846f,141.46277f,141.26692f,141.07094f,140.8748f,140.67853f,140.48209f,140.28552f,140.08879f,139.89192f,139.6949f,139.49774f,139.30043f,139.10297f,138.90535f,138.7076f,138.5097f,138.31165f,138.11345f,137.9151f,137.71661f,137.51797f,137.31918f,137.12025f,136.92117f,136.72194f,136.52255f,136.32303f,136.12337f,135.92354f,135.72357f,135.52345f,135.3232f,135.12279f,134.92223f,134.72151f,134.52066f,134.31966f,134.1185f,133.9172f,133.71576f,133.51418f,133.31242f,133.11053f,132.9085f,132.70631f,132.50398f,132.3015f,132.09888f,131.89609f,131.69316f,131.4901f,131.28687f,131.0835f,130.87999f,130.67632f,130.4725f,130.26854f,130.06442f,129.86017f,129.65576f,129.4512f,129.24649f,129.04164f,128.83664f,128.63148f,128.42618f,128.22073f,128.01514f,127.809395f,127.6035f,127.39745f,127.19126f,126.984924f,126.77843f,126.57179f,126.365005f,126.158066f,125.95098f,125.743744f,125.536354f,125.32882f,125.12114f,124.91331f,124.70532f,124.49719f,124.28891f,124.08048f,123.8719f,123.66317f,123.45429f,123.24526f,123.03608f,122.82675f,122.61727f,122.407646f,122.19787f,121.98794f,121.777855f,121.56763f,121.357254f,121.14672f,120.93604f,120.72521f,120.51423f,120.3031f,120.09182f,119.88039f,119.66881f,119.45708f,119.24519f,119.03316f,118.82097f,118.608635f,118.39615f,118.18351f,117.970726f,117.75778f,117.54469f,117.33145f,117.11806f,116.90451f,116.69082f,116.47697f,116.26297f,116.04882f,115.83452f,115.62006f,115.405464f,115.190704f,114.9758f,114.760735f,114.545525f,114.33016f,114.11465f,113.89898f,113.68316f,113.467186f,113.25106f,113.03478f,112.81836f,112.601776f,112.38504f,112.16815f,111.95112f,111.733925f,111.51659f,111.29909f,111.081436f,110.86364f,110.64568f,110.427574f,110.20932f,109.990906f,109.77234f,109.55362f,109.33475f,109.11572f,108.896545f,108.677216f,108.45773f,108.2381f,108.01831f,107.79837f,107.57827f,107.358025f,107.13762f,106.91706f,106.69636f,106.475494f,106.25448f,106.03331f,105.81199f,105.590515f,105.36888f,105.1471f,104.92517f,104.70308f,104.480835f,104.258446f,104.0359f,103.813194f,103.59034f,103.36733f,103.14417f,102.92086f,102.697395f,102.47378f,102.25001f,102.02608f,101.802f,101.577774f,101.35339f,101.12886f,100.90417f,100.67933f,100.45434f,100.229195f,100.00391f,99.77846f,99.55286f,99.32711f,99.10121f,98.87516f,98.64896f,98.4226f,98.1961f,97.969444f,97.742645f,97.515686f,97.28858f,97.06133f,96.83393f,96.606384f,96.378685f,96.15084f,95.922844f,95.69471f,95.46642f,95.23798f,95.00941f,94.78068f,94.55181f,94.32279f,94.093636f,93.864334f,93.63489f,93.4053f,93.17557f,92.9457f,92.71569f,92.485535f,92.25524f,92.02482f,91.79425f,91.563545f,91.3327f,91.10173f,90.87062f,90.63938f,90.408005f,90.1765f,89.94486f,89.713104f,89.48121f,89.24919f,89.01705f,88.78478f,88.55239f,88.31988f,88.08725f,87.8545f,87.621635f,87.38866f,87.15557f,86.92236f,86.68906f,86.455635f,86.22211f,85.98848f,85.75475f,85.52093f,85.28701f,85.05299f,84.818886f,84.584694f,84.35041f,84.11605f,83.88161f,83.64709f,83.4125f,83.17784f,82.94311f,82.70832f,82.47347f,82.23857f,82.00362f,81.768616f,81.53357f,81.29849f,81.06338f,80.82823f,80.59307f,80.35788f,80.12269f,79.88748f,79.652275f,79.417076f,79.181885f,78.94672f,78.71156f,78.47645f,78.24136f,78.006325f,77.77134f,77.536415f,77.30156f,77.06678f,76.83209f,76.59749f,76.36299f,76.12861f,75.89434f,75.66021f,75.42622f,75.19239f,74.95872f,74.72522f,74.491905f,74.2588f,74.025894f,73.79321f,73.560776f,73.32858f,73.09665f,72.865f,72.63363f,72.40257f,72.17184f,71.94144f,71.71138f,71.4817f,71.252396f,71.0235f,70.79501f,70.56696f,70.33937f,70.112236f,69.8856f,69.65947f,69.43386f,69.20879f,68.9843f,68.760376f,68.53706f,68.31438f,68.09233f,67.87095f,67.65025f,67.43027f,67.211f,66.992485f,66.77474f,66.557785f,66.34164f,66.12633f,65.91187f,65.69829f,65.4856f,65.27384f,65.06301f,64.85314f,64.64426f,64.43636f,64.2295f,64.023674f,63.818913f,63.615227f,63.412643f,63.211174f,63.010845f,62.81167f,62.613663f,62.416843f,62.22123f,62.026836f,61.83368f,61.641773f,61.451126f,61.26176f,61.07369f,60.886913f,60.701458f,60.517323f,60.334526f,60.153072f,59.972973f,59.794235f,59.616867f,59.440872f,59.266262f,59.093037f,58.921204f,58.750767f,58.581726f,58.41409f,58.247852f,58.083023f,57.919594f,57.757572f,57.59695f,57.437733f,57.279915f,57.123493f,56.968468f,56.81483f,56.662582f,56.51171f,56.362217f,56.214096f,56.067337f,55.921936f,55.77788f,55.635174f,55.4938f,55.353752f,55.215023f,55.077602f,54.941483f,54.806656f,54.673103f,54.540825f,54.40981f,54.28004f,54.151512f,54.02421f,53.89813f,53.773254f,53.649574f,53.527077f,53.40575f,53.285587f,53.166573f,53.048695f,52.931942f,52.816303f,52.701767f,52.58832f,52.475952f,52.36465f,52.254402f,52.145195f,52.03702f,51.929867f,51.823715f,51.718563f,51.614395f,51.511196f,51.40896f,51.30767f,51.207317f,51.107895f,51.009384f,50.911777f,50.815067f,50.719234f,50.624275f,50.530174f,50.436924f,50.344513f,50.25293f,50.162167f,50.07221f,49.983047f,49.894676f,49.807083f,49.720257f,49.634186f,49.548866f,49.464287f,49.380436f,49.297306f,49.214886f,49.13317f,49.052147f,48.97181f,48.892147f,48.813152f,48.734818f,48.657135f,48.580093f,48.503685f,48.427902f,48.35274f,48.27819f,48.204243f,48.13089f,48.058125f,47.98594f,47.91433f,47.843285f,47.7728f,47.70287f,47.633484f,47.564636f,47.496323f,47.42853f,47.361263f,47.294506f,47.228256f,47.162506f,47.09725f,47.032486f,46.9682f,46.904392f,46.841057f,46.778187f,46.715775f,46.653816f,46.592308f,46.531246f,46.47062f,46.410427f,46.350662f,46.29132f,46.232395f,46.173885f,46.115784f,46.058086f,46.000786f,45.943882f,45.887367f,45.83124f,45.775494f,45.720123f,45.665123f,45.610493f,45.55623f,45.502323f,45.448772f,45.395576f,45.342728f,45.290222f,45.238056f,45.18623f,45.134735f,45.083572f,45.032734f,44.982216f,44.93202f,44.882137f,44.83257f,44.783306f,44.734352f,44.6857f,44.637344f,44.589287f,44.541523f,44.494045f,44.446857f,44.399952f,44.35333f,44.306984f,44.26091f,44.21511f,44.169582f,44.12432f,44.07932f,44.034584f,43.990105f,43.94588f,43.901913f,43.858192f,43.814724f,43.7715f,43.72852f,43.685783f,43.643284f,43.60102f,43.55899f,43.517193f,43.475628f,43.434288f,43.393173f,43.352283f,43.31161f,43.27116f,43.230927f,43.190907f,43.1511f,43.111507f,43.07212f,43.03294f,42.993965f,42.955196f,42.916626f,42.878254f,42.84008f,42.802105f,42.76432f,42.72673f,42.68933f,42.65212f,42.615097f,42.57826f,42.541603f,42.50513f,42.468838f,42.432728f,42.39679f,42.36103f,42.325447f,42.290035f,42.254795f,42.219723f,42.184822f,42.150085f,42.115517f,42.081112f,42.046867f,42.012787f,41.978867f,41.945103f,41.9115f,41.87805f,41.844757f,41.811615f,41.778625f,41.74579f,41.7131f,41.680565f,41.64817f,41.615925f,41.583824f,41.55187f,41.520054f,41.48838f,41.45685f,41.425457f,41.394203f,41.363083f,41.3321f,41.301254f,41.270542f,41.23996f,41.209515f,41.179195f,41.149006f,41.118946f,41.089016f,41.05921f,41.029533f,40.999977f,40.970547f,40.941242f,40.912056f,40.88299f,40.854046f,40.825222f,40.796516f,40.76793f,40.739456f,40.7111f,40.682858f,40.65473f,40.62672f,40.598816f,40.57103f,40.54335f,40.51578f,40.48832f,40.46097f,40.433727f,40.40659f,40.379562f,40.352634f,40.325817f,40.2991f,40.272488f,40.245975f,40.219566f,40.19326f,40.167053f,40.140945f,40.114937f,40.089027f,40.063213f,40.0375f,40.01188f,39.986355f,39.960926f,39.935593f,39.91035f,39.885204f,39.86015f,39.835186f,39.810314f,39.78553f,39.76084f,39.736237f,39.711723f,39.687298f,39.662964f,39.63871f,39.614548f,39.59047f,39.56648f,39.542572f,39.518745f,39.495007f,39.47135f,39.447777f,39.424286f,39.40088f,39.37755f,39.3543f,39.33113f,39.308044f,39.285034f,39.262104f,39.239254f,39.216476f,39.19378f,39.171158f,39.148617f,39.126144f,39.103752f,39.081432f,39.05919f,39.037018f,39.014923f,38.992897f,38.970947f,38.949066f,38.927258f,38.90552f,38.883858f,38.862263f,38.840736f,38.819283f,38.797894f,38.776577f,38.75533f,38.734146f,38.71303f,38.691986f,38.671005f,38.650093f,38.629246f,38.608463f,38.587746f,38.567097f,38.54651f,38.525986f,38.505527f,38.48513f,38.464798f,38.444527f,38.42432f,38.404175f,38.38409f,38.364067f,38.344105f,38.324203f,38.304363f,38.28458f,38.264862f,38.245197f,38.225594f,38.20605f,38.186565f,38.167137f,38.147766f,38.128452f,38.109196f,38.089996f,38.070854f,38.051765f,38.032734f,38.01376f,37.99484f,37.97597f,37.95716f,37.938404f,37.9197f,37.901054f,37.882458f,37.863914f,37.84543f,37.826992f,37.808605f,37.790276f,37.77199f,37.753765f,37.735588f,37.71746f,37.699387f,37.681362f,37.663387f,37.645462f,37.627586f,37.60976f,37.591984f,37.574257f,37.556576f,37.538948f,37.521366f,37.50383f,37.486343f,37.468906f,37.451515f,37.43417f,37.41687f,37.39962f,37.382416f,37.365257f,37.348145f,37.331078f,37.314056f,37.297077f,37.280148f,37.26326f,37.246418f,37.22962f,37.212868f,37.196156f,37.179493f,37.16287f,37.14629f,37.129753f,37.11326f,37.09681f,37.080402f,37.064034f,37.04771f,37.03143f,37.015186f,36.99899f,36.98283f,36.966713f,36.950638f,36.934605f,36.91861f,36.902657f,36.88674f,36.87087f,36.855034f,36.839237f,36.823483f,36.807766f,36.792088f,36.77645f,36.760853f,36.74529f,36.729767f,36.714283f,36.698837f,36.683426f,36.668056f,36.65272f,36.637424f,36.622166f,36.606945f,36.59176f,36.57661f,36.561497f,36.54642f,36.53138f,36.516376f,36.501408f,36.486477f,36.471577f,36.456715f,36.44189f,36.427097f,36.412342f,36.39762f,36.382935f,36.368282f,36.35366f,36.339077f,36.324528f,36.310013f,36.29553f,36.28108f,36.266663f,36.25228f,36.23793f,36.223618f,36.20933f,36.195084f,36.180862f,36.16668f,36.152523f,36.138405f,36.124313f,36.110256f,36.096233f,36.082237f,36.068275f,36.054344f,36.040443f,36.026577f,36.012737f,35.99893f,35.985157f,35.971413f,35.957695f,35.94401f,35.93036f,35.916733f,35.903137f,35.889576f,35.87604f,35.862534f,35.84906f,35.835613f,35.822197f,35.808807f,35.795452f,35.78212f,35.76882f,35.755547f,35.742306f,35.729088f,35.715904f,35.702744f,35.689613f,35.67651f,35.663437f,35.65039f,35.63737f,35.62438f,35.611416f,35.59848f,35.585567f,35.572685f,35.559834f,35.547005f,35.534203f,35.521427f,35.50868f,35.495956f,35.48326f,35.470592f,35.45795f,35.445335f,35.432743f,35.42018f,35.407642f,35.39513f,35.38264f,35.37018f,35.357742f,35.345333f,35.332947f,35.320587f,35.30825f,35.29594f,35.283653f,35.271393f,35.25916f,35.246944f,35.23476f,35.222595f,35.210457f,35.19834f,35.186253f,35.174187f,35.162144f,35.150127f,35.138134f,35.126163f,35.114216f,35.10229f,35.090393f,35.078514f,35.06666f,35.054832f,35.043026f,35.03124f,35.019478f,35.00774f,34.996025f,34.984333f,34.972664f,34.961014f,34.94939f,34.937786f,34.926205f,34.914646f,34.90311f,34.891598f,34.880104f,34.868637f,34.857185f,34.84576f,34.834354f,34.82297f,34.81161f,34.80027f,34.78895f,34.777653f,34.766376f,34.75512f,34.743885f,34.732674f,34.72148f,34.710308f,34.699158f,34.68803f,34.676918f,34.665833f,34.654762f,34.643715f,34.632687f,34.62168f,34.610695f,34.599728f,34.58878f,34.577854f,34.566948f,34.55606f,34.545193f,34.534344f,34.523518f,34.51271f,34.501923f,34.491154f,34.480404f,34.469673f,34.45896f,34.448273f,34.4376f,34.426945f,34.41631f,34.405697f,34.3951f,34.38452f,34.373962f,34.363422f,34.3529f,34.3424f,34.331917f,34.32145f,34.311005f,34.300575f,34.290165f,34.27977f,34.269398f,34.25904f,34.248703f,34.238384f,34.22808f,34.217796f,34.20753f,34.197285f,34.187054f,34.176838f,34.166645f,34.156467f,34.146305f,34.13616f,34.126038f,34.11593f,34.10584f,34.095764f,34.08571f,34.07567f,34.065647f,34.05564f,34.045654f,34.035683f,34.02573f,34.015793f,34.00587f,33.995968f,33.98608f,33.976208f,33.966354f,33.956516f,33.946693f,33.93689f,33.9271f,33.917328f,33.90757f,33.89783f,33.888107f,33.8784f,33.868706f,33.85903f,33.849373f,33.83973f,33.8301f,33.820488f,33.81089f,33.80131f,33.791744f,33.782196f,33.772663f,33.76314f,33.75364f,33.744152f,33.73468f,33.725224f,33.715782f,33.706356f,33.696945f,33.68755f,33.67817f,33.6688f,33.65945f,33.650116f,33.640793f,33.63149f,33.622196f,33.612923f,33.60366f,33.594414f,33.58518f,33.575962f,33.56676f,33.55757f,33.548397f,33.539238f,33.53009f,33.52096f,33.511845f,33.502743f,33.493656f,33.48458f,33.47552f,33.466476f,33.457443f,33.44843f,33.439423f,33.430435f,33.42146f,33.4125f,33.40355f,33.394615f,33.385696f,33.37679f,33.367897f,33.359016f,33.35015f,33.3413f,33.332462f,33.32364f,33.314827f,33.306026f,33.297245f,33.28847f,33.279713f,33.27097f,33.262238f,33.25352f,33.244816f,33.236122f,33.227444f,33.218777f,33.210125f,33.201485f,33.19286f,33.184246f,33.175644f,33.167053f,33.158478f,33.149918f,33.141365f,33.132828f,33.124306f,33.11579f,33.107292f,33.098804f,33.090332f,33.08187f,33.07342f,33.064983f,33.05656f,33.048145f,33.039745f,33.03136f,33.022984f,33.01462f,33.006268f,32.99793f,32.989605f,32.98129f,32.97299f,32.964695f,32.956417f,32.94815f,32.939896f,32.931652f,32.923424f,32.915203f,32.906998f,32.8988f,32.890617f,32.882446f,32.874283f,32.866135f,32.857998f,32.849873f,32.84176f,32.833656f,32.825565f,32.817486f,32.809418f,32.80136f,32.793316f,32.785282f,32.77726f,32.76925f,32.76125f,32.75326f,32.74528f,32.737316f,32.72936f,32.721416f,32.71348f,32.705563f,32.69765f,32.68975f,32.68186f,32.673985f,32.666115f,32.65826f,32.650414f,32.64258f,32.634754f,32.62694f,32.61914f,32.61135f,32.60357f,32.5958f,32.58804f,32.58029f,32.57255f,32.564823f,32.557106f,32.5494f,32.541702f,32.53402f,32.526344f,32.518677f,32.511024f,32.50338f,32.495747f,32.48812f,32.48051f,32.472904f,32.465313f,32.45773f,32.450157f,32.442596f,32.435043f,32.4275f,32.41997f,32.41245f,32.404938f,32.397434f,32.389942f,32.38246f,32.37499f,32.367527f,32.360073f,32.35263f,32.3452f,32.337776f,32.330364f,32.32296f,32.315567f,32.30818f,32.300808f,32.293446f,32.28609f,32.278744f,32.271408f,32.264084f,32.256767f,32.249462f,32.242165f,32.234875f,32.227596f,32.22033f,32.21307f,32.20582f,32.198578f,32.191345f,32.184124f,32.17691f,32.16971f,32.162514f,32.155327f,32.14815f,32.140987f,32.133827f,32.12668f,32.11954f,32.11241f,32.10529f,32.09818f,32.091076f,32.08398f,32.076897f,32.06982f,32.06275f,32.055695f,32.048645f,32.041607f,32.034576f,32.027554f,32.02054f,32.013535f,32.00654f,31.999552f,31.992573f,31.985603f,31.978642f,31.97169f,31.964746f,31.957811f,31.950886f,31.943968f,31.937057f,31.930157f,31.923265f,31.916382f,31.909508f,31.902641f,31.895782f,31.888933f,31.882092f,31.87526f,31.868435f,31.86162f,31.854813f,31.848013f,31.841223f,31.83444f,31.827665f,31.8209f,31.814142f,31.807392f,31.800652f,31.793919f,31.787193f,31.780478f,31.773767f,31.767069f,31.760376f,31.75369f,31.747015f,31.740347f,31.733686f,31.727036f,31.72039f,31.713755f,31.707127f,31.700506f,31.693895f,31.68729f,31.680695f,31.674105f,31.667524f,31.660952f,31.654387f,31.64783f,31.64128f,31.634739f,31.628204f,31.621677f,31.61516f,31.608648f,31.602146f,31.595652f,31.589163f,31.582684f,31.57621f,31.569746f,31.563288f,31.556839f,31.550396f,31.543962f,31.537535f,31.531115f,31.524704f,31.5183f,31.511902f,31.505512f,31.49913f,31.492756f,31.486387f,31.480028f,31.473675f,31.467329f,31.46099f,31.45466f,31.448338f,31.442022f,31.435713f,31.42941f,31.423117f,31.41683f,31.41055f,31.404278f,31.398012f,31.391754f,31.385502f,31.37926f,31.373022f,31.366793f,31.360569f,31.354353f,31.348145f,31.341944f,31.335749f,31.329561f,31.323381f,31.317207f,31.31104f,31.304882f,31.298729f,31.292583f,31.286444f,31.280313f,31.274187f,31.26807f,31.26196f,31.255854f,31.249756f,31.243666f,31.237581f,31.231504f,31.225435f,31.219372f,31.213314f,31.207264f,31.201221f,31.195185f,31.189154f,31.18313f,31.177114f,31.171104f,31.1651f,31.159103f,31.153114f,31.147131f,31.141153f,31.135183f,31.12922f,31.123262f,31.117311f,31.111368f,31.105429f,31.099499f,31.093575f,31.087656f,31.081743f,31.075838f,31.069939f,31.064047f,31.05816f,31.05228f,31.046408f,31.04054f,31.034681f,31.028826f,31.022978f,31.017138f,31.011301f,31.005472f,30.99965f,30.993834f,30.988024f,30.98222f,30.976421f,30.970629f,30.964844f,30.959064f,30.953293f,30.947525f,30.941765f};
class mydspSIG1 {
	
  private:
	
	int fmydspSIG1Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG1() {
		return 0;
	}
	int getNumOutputsmydspSIG1() {
		return 1;
	}
	
	void instanceInitmydspSIG1(int sample_rate) {
		fmydspSIG1Wave0_idx = 0;
	}
	
	void fillmydspSIG1(int count, float* table) {
		for (int i2 = 0; i2 < count; i2 = i2 + 1) {
			table[i2] = fmydspSIG1Wave0[fmydspSIG1Wave0_idx];
			fmydspSIG1Wave0_idx = (1 + fmydspSIG1Wave0_idx) % 2001;
		}
	}

};

static mydspSIG1* newmydspSIG1() { return (mydspSIG1*)new mydspSIG1(); }
static void deletemydspSIG1(mydspSIG1* dsp) { delete dsp; }

const static float fmydspSIG2Wave0[2001] = {127.202255f,127.14474f,127.08721f,127.02967f,126.97213f,126.91458f,126.857025f,126.79946f,126.74189f,126.68431f,126.626724f,126.56913f,126.511536f,126.453926f,126.39631f,126.338684f,126.28106f,126.22342f,126.16577f,126.10812f,126.05046f,125.9928f,125.93513f,125.87744f,125.819756f,125.76206f,125.70436f,125.64665f,125.588936f,125.53121f,125.47348f,125.41574f,125.357994f,125.30025f,125.242485f,125.18472f,125.126945f,125.06917f,125.01138f,124.95359f,124.89579f,124.83798f,124.78017f,124.72234f,124.66451f,124.60668f,124.548836f,124.49099f,124.43313f,124.37527f,124.3174f,124.25952f,124.20164f,124.143745f,124.08585f,124.02795f,123.97004f,123.91212f,123.854195f,123.796265f,123.73833f,123.68038f,123.62243f,123.564476f,123.50651f,123.44854f,123.390564f,123.33258f,123.27459f,123.21659f,123.158585f,123.10058f,123.04256f,122.984535f,122.926506f,122.86847f,122.810425f,122.75238f,122.69432f,122.63626f,122.57819f,122.52012f,122.462036f,122.403946f,122.34585f,122.28775f,122.229645f,122.17153f,122.11341f,122.05528f,121.997154f,121.93902f,121.88087f,121.822716f,121.764565f,121.7064f,121.64823f,121.59006f,121.531876f,121.47369f,121.41549f,121.35729f,121.29908f,121.24087f,121.182655f,121.12443f,121.0662f,121.00796f,120.949715f,120.89147f,120.83321f,120.774956f,120.71669f,120.65842f,120.600136f,120.541855f,120.48357f,120.42527f,120.36697f,120.308655f,120.25034f,120.192024f,120.1337f,120.07537f,120.01703f,119.95869f,119.900345f,119.84199f,119.78363f,119.725266f,119.66689f,119.60851f,119.55013f,119.491745f,119.43336f,119.374954f,119.31655f,119.25814f,119.19972f,119.141304f,119.08288f,119.024445f,118.96601f,118.90756f,118.84911f,118.790665f,118.7322f,118.67374f,118.61527f,118.55679f,118.498314f,118.43983f,118.38134f,118.32284f,118.264336f,118.20583f,118.147316f,118.0888f,118.03028f,117.97175f,117.913216f,117.85468f,117.796135f,117.73759f,117.67904f,117.620476f,117.56191f,117.50335f,117.44477f,117.38619f,117.327614f,117.26902f,117.21043f,117.15183f,117.09322f,117.03462f,116.976006f,116.91739f,116.858765f,116.80014f,116.74151f,116.68287f,116.62423f,116.56558f,116.50693f,116.44827f,116.38961f,116.33095f,116.27228f,116.2136f,116.15492f,116.09624f,116.03755f,115.97886f,115.92016f,115.86146f,115.80275f,115.74404f,115.685326f,115.62661f,115.56788f,115.509155f,115.450424f,115.391685f,115.33294f,115.27419f,115.21545f,115.15669f,115.09793f,115.03917f,114.9804f,114.92163f,114.862854f,114.80408f,114.74529f,114.6865f,114.62771f,114.56891f,114.51011f,114.45131f,114.3925f,114.33369f,114.27487f,114.21605f,114.15723f,114.0984f,114.039566f,113.98073f,113.92189f,113.863045f,113.8042f,113.745346f,113.68649f,113.62763f,113.56876f,113.5099f,113.45103f,113.39216f,113.33328f,113.2744f,113.215515f,113.156624f,113.09773f,113.03884f,112.979935f,112.921036f,112.86213f,112.80322f,112.74431f,112.685394f,112.62647f,112.56755f,112.50862f,112.44969f,112.39076f,112.331825f,112.27288f,112.21394f,112.15499f,112.09605f,112.037094f,111.978134f,111.919174f,111.860214f,111.801254f,111.74228f,111.68331f,111.62434f,111.56536f,111.50638f,111.447395f,111.38841f,111.32942f,111.27043f,111.21143f,111.152435f,111.09344f,111.03443f,110.975426f,110.91642f,110.85741f,110.79839f,110.73938f,110.68036f,110.62134f,110.56231f,110.50329f,110.44426f,110.38522f,110.32619f,110.26715f,110.208115f,110.14907f,110.09003f,110.030975f,109.97193f,109.91288f,109.85382f,109.79477f,109.73571f,109.67665f,109.617584f,109.55852f,109.49945f,109.440384f,109.38131f,109.322235f,109.26316f,109.20409f,109.145004f,109.08592f,109.02684f,108.96775f,108.90866f,108.84957f,108.79048f,108.73139f,108.672295f,108.6132f,108.5541f,108.494995f,108.4359f,108.37679f,108.31769f,108.25858f,108.19947f,108.14036f,108.081245f,108.02213f,107.96302f,107.9039f,107.84479f,107.78567f,107.72655f,107.66742f,107.6083f,107.54917f,107.49005f,107.43092f,107.371796f,107.31266f,107.25353f,107.1944f,107.13527f,107.07613f,107.017f,106.957855f,106.89872f,106.839584f,106.78044f,106.721306f,106.66216f,106.60302f,106.54388f,106.48473f,106.42558f,106.36644f,106.3073f,106.248146f,106.188995f,106.12985f,106.0707f,106.01155f,105.9524f,105.89325f,105.8341f,105.77495f,105.71579f,105.65664f,105.59749f,105.53833f,105.47918f,105.42002f,105.36087f,105.30171f,105.24256f,105.1834f,105.124245f,105.065094f,105.005936f,104.94678f,104.88762f,104.82847f,104.76931f,104.71015f,104.65099f,104.59184f,104.532684f,104.473526f,104.414375f,104.35522f,104.29606f,104.23691f,104.17775f,104.1186f,104.05944f,104.00029f,103.94114f,103.88198f,103.82283f,103.76368f,103.70453f,103.64538f,103.58623f,103.52708f,103.467926f,103.408775f,103.34963f,103.29048f,103.23134f,103.172195f,103.113045f,103.0539f,102.99476f,102.93562f,102.87648f,102.81734f,102.7582f,102.699066f,102.63992f,102.58079f,102.52165f,102.462524f,102.40339f,102.34426f,102.28513f,102.226006f,102.16688f,102.10775f,102.04862f,101.9895f,101.93038f,101.87126f,101.81214f,101.75303f,101.69391f,101.634796f,101.57568f,101.51658f,101.457466f,101.39836f,101.33926f,101.28015f,101.221054f,101.16196f,101.10286f,101.04376f,100.98467f,100.925575f,100.86649f,100.8074f,100.748314f,100.68923f,100.63016f,100.571075f,100.512f,100.45293f,100.39385f,100.334785f,100.27572f,100.21666f,100.15759f,100.09853f,100.03948f,99.98042f,99.92137f,99.86233f,99.803276f,99.74423f,99.685196f,99.62616f,99.56712f,99.50809f,99.44906f,99.39004f,99.33101f,99.27199f,99.212975f,99.15396f,99.09495f,99.03594f,98.97694f,98.91794f,98.85894f,98.79994f,98.74095f,98.68196f,98.62298f,98.563995f,98.50502f,98.446045f,98.38707f,98.3281f,98.26914f,98.21018f,98.15122f,98.09227f,98.033325f,97.97438f,97.915436f,97.8565f,97.79757f,97.73864f,97.67971f,97.62079f,97.561874f,97.50296f,97.44405f,97.38515f,97.32625f,97.26735f,97.20846f,97.14957f,97.09068f,97.03181f,96.97293f,96.91406f,96.855194f,96.79633f,96.73748f,96.67863f,96.61978f,96.560936f,96.5021f,96.44327f,96.38444f,96.325615f,96.26679f,96.207985f,96.14917f,96.09037f,96.03157f,95.97278f,95.91399f,95.85521f,95.796425f,95.737656f,95.67889f,95.620125f,95.56137f,95.50262f,95.44387f,95.38513f,95.3264f,95.26767f,95.20895f,95.15023f,95.091515f,95.03281f,94.974106f,94.91541f,94.85673f,94.79804f,94.739365f,94.680695f,94.62203f,94.56337f,94.504715f,94.44607f,94.38743f,94.3288f,94.270164f,94.21155f,94.15293f,94.09432f,94.03572f,93.97712f,93.91853f,93.85995f,93.80138f,93.742805f,93.68424f,93.62569f,93.56713f,93.50859f,93.45006f,93.391525f,93.333f,93.27449f,93.21598f,93.15748f,93.09898f,93.0405f,92.98202f,92.923546f,92.86508f,92.80662f,92.74817f,92.68973f,92.631294f,92.57286f,92.51444f,92.45603f,92.39762f,92.339226f,92.28084f,92.22245f,92.16408f,92.10571f,92.047356f,91.989f,91.93066f,91.87232f,91.813995f,91.75568f,91.697365f,91.63906f,91.58077f,91.522484f,91.4642f,91.40594f,91.34768f,91.28942f,91.23118f,91.17294f,91.114716f,91.056496f,90.99829f,90.94009f,90.8819f,90.823715f,90.76554f,90.707375f,90.649216f,90.591064f,90.53293f,90.4748f,90.41668f,90.35857f,90.30047f,90.24237f,90.18429f,90.12621f,90.06815f,90.01009f,89.95205f,89.89401f,89.83599f,89.77797f,89.71996f,89.661964f,89.60398f,89.546f,89.48803f,89.43008f,89.37212f,89.314186f,89.256256f,89.19834f,89.140434f,89.082535f,89.02465f,88.966774f,88.908905f,88.85104f,88.793205f,88.73537f,88.67754f,88.61973f,88.56192f,88.50413f,88.44635f,88.38858f,88.33082f,88.27306f,88.215324f,88.1576f,88.099884f,88.042175f,87.98448f,87.926796f,87.869125f,87.81147f,87.753815f,87.69618f,87.63855f,87.58094f,87.52333f,87.465744f,87.408165f,87.35059f,87.29304f,87.23549f,87.177956f,87.12044f,87.06293f,87.00543f,86.947945f,86.89047f,86.833015f,86.775566f,86.718124f,86.660706f,86.603294f,86.54589f,86.48851f,86.43114f,86.37377f,86.31642f,86.25909f,86.20177f,86.144455f,86.08716f,86.02988f,85.9726f,85.91535f,85.85811f,85.80087f,85.74366f,85.686455f,85.629265f,85.57208f,85.51492f,85.45777f,85.400635f,85.34351f,85.28641f,85.22931f,85.17223f,85.115166f,85.05811f,85.001076f,84.94405f,84.88705f,84.83005f,84.77307f,84.7161f,84.65916f,84.60222f,84.545296f,84.488396f,84.4315f,84.374626f,84.31777f,84.260925f,84.204094f,84.147285f,84.090485f,84.03371f,83.97694f,83.92019f,83.86346f,83.80674f,83.75004f,83.69335f,83.63668f,83.58003f,83.5234f,83.46678f,83.41018f,83.3536f,83.297035f,83.240486f,83.18395f,83.12744f,83.070946f,83.01447f,82.95801f,82.90157f,82.84515f,82.78875f,82.73237f,82.676f,82.61965f,82.56333f,82.50703f,82.45074f,82.39447f,82.33823f,82.282005f,82.2258f,82.16962f,82.11346f,82.05732f,82.0012f,81.94511f,81.88903f,81.83298f,81.776955f,81.72095f,81.66496f,81.60901f,81.55308f,81.49717f,81.441284f,81.38543f,81.3296f,81.27379f,81.21801f,81.16226f,81.10654f,81.050835f,80.99517f,80.93953f,80.88392f,80.82834f,80.77278f,80.71726f,80.66177f,80.60632f,80.550896f,80.49551f,80.44015f,80.38483f,80.32954f,80.27429f,80.21908f,80.163895f,80.10876f,80.05366f,79.998604f,79.94358f,79.8886f,79.83367f,79.77878f,79.72393f,79.669136f,79.61438f,79.55967f,79.50501f,79.45041f,79.39585f,79.34135f,79.286896f,79.232506f,79.17816f,79.123886f,79.069664f,79.0155f,78.9614f,78.90737f,78.8534f,78.79951f,78.745674f,78.69191f,78.63822f,78.58461f,78.531075f,78.477615f,78.42424f,78.37094f,78.31773f,78.26461f,78.21158f,78.15864f,78.1058f,78.05305f,78.0004f,77.94785f,77.89541f,77.84308f,77.790855f,77.73875f,77.68675f,77.63487f,77.58312f,77.531494f,77.48f,77.428635f,77.3774f,77.32632f,77.27537f,77.22456f,77.17391f,77.12341f,77.073074f,77.022896f,76.97288f,76.92303f,76.87335f,76.82385f,76.77453f,76.725395f,76.676445f,76.62769f,76.57913f,76.53077f,76.48261f,76.43467f,76.38693f,76.33942f,76.29212f,76.24505f,76.198204f,76.151596f,76.105225f,76.05909f,76.0132f,75.96756f,75.92218f,75.877045f,75.83218f,75.787575f,75.74323f,75.699165f,75.65537f,75.611855f,75.56862f,75.525665f,75.483f,75.44063f,75.39854f,75.35675f,75.31526f,75.27407f,75.23318f,75.19259f,75.15231f,75.112335f,75.07268f,75.033325f,74.994286f,74.95556f,74.917145f,74.87905f,74.84128f,74.80381f,74.76667f,74.72984f,74.69334f,74.65714f,74.62128f,74.585724f,74.55048f,74.51557f,74.480965f,74.44668f,74.41271f,74.37905f,74.34571f,74.312675f,74.27995f,74.24754f,74.21544f,74.18364f,74.152145f,74.12096f,74.090065f,74.05947f,74.029175f,73.999176f,73.96947f,73.94005f,73.91092f,73.88207f,73.85351f,73.825226f,73.797226f,73.76949f,73.74204f,73.71486f,73.68794f,73.66129f,73.6349f,73.60878f,73.58291f,73.5573f,73.53193f,73.50681f,73.48195f,73.45732f,73.432945f,73.4088f,73.38489f,73.36121f,73.33776f,73.314545f,73.29155f,73.268776f,73.24622f,73.223885f,73.20177f,73.179855f,73.15815f,73.13665f,73.11536f,73.09427f,73.07337f,73.05267f,73.032166f,73.011856f,72.99173f,72.97179f,72.952034f,72.93246f,72.91306f,72.893845f,72.874794f,72.85593f,72.83722f,72.81869f,72.800316f,72.78211f,72.76406f,72.74618f,72.72845f,72.71088f,72.69345f,72.67618f,72.65906f,72.64208f,72.62525f,72.60857f,72.59202f,72.575615f,72.55935f,72.54321f,72.527214f,72.51135f,72.49561f,72.48001f,72.46453f,72.44918f,72.43395f,72.418846f,72.40386f,72.38899f,72.374245f,72.35961f,72.34509f,72.330696f,72.3164f,72.30222f,72.288155f,72.274185f,72.26033f,72.24658f,72.23293f,72.21939f,72.20594f,72.192604f,72.17936f,72.16621f,72.15316f,72.140205f,72.12734f,72.11458f,72.1019f,72.08932f,72.07682f,72.064415f,72.05209f,72.03986f,72.02772f,72.015656f,72.00368f,71.991776f,71.979965f,71.96823f,71.95657f,71.945f,71.9335f,71.92208f,71.910736f,71.89947f,71.888275f,71.87715f,71.866104f,71.85513f,71.84422f,71.83339f,71.822624f,71.81193f,71.8013f,71.79074f,71.78024f,71.76981f,71.75945f,71.74915f,71.738914f,71.728745f,71.718636f,71.70859f,71.69859f,71.68867f,71.6788f,71.66899f,71.65924f,71.64955f,71.639915f,71.63034f,71.62081f,71.61135f,71.60194f,71.592575f,71.583275f,71.57403f,71.56483f,71.55568f,71.546585f,71.537544f,71.52855f,71.51961f,71.51072f,71.50188f,71.49308f,71.48434f,71.47564f,71.46699f,71.45838f,71.44982f,71.44131f,71.432846f,71.42442f,71.416046f,71.407715f,71.39942f,71.391174f,71.38297f,71.37482f,71.3667f,71.35862f,71.350586f,71.34259f,71.33464f,71.32672f,71.31885f,71.31102f,71.30322f,71.29546f,71.28775f,71.28007f,71.27243f,71.264824f,71.257256f,71.249725f,71.24223f,71.23477f,71.227356f,71.21996f,71.212616f,71.2053f,71.19801f,71.190765f,71.183556f,71.17638f,71.16923f,71.16212f,71.15504f,71.14799f,71.14097f,71.13399f,71.12704f,71.12012f,71.11323f,71.10637f,71.09954f,71.09274f,71.085976f,71.07924f,71.07253f,71.06586f,71.05921f,71.05259f,71.046f,71.03944f,71.032906f,71.0264f,71.01992f,71.01347f,71.00705f,71.00065f,70.994286f,70.987946f,70.98163f,70.97534f,70.96908f,70.96284f,70.95663f,70.95045f,70.94428f,70.93815f,70.932045f,70.92596f,70.9199f,70.913864f,70.90785f,70.90186f,70.8959f,70.88996f,70.88405f,70.87815f,70.872284f,70.86644f,70.86061f,70.85481f,70.84904f,70.84328f,70.83755f,70.83183f,70.82615f,70.82048f,70.814835f,70.809204f,70.803604f,70.79802f,70.79246f,70.78692f,70.7814f,70.7759f,70.770424f,70.76496f,70.75952f,70.754105f,70.74871f,70.74333f,70.73797f,70.73263f,70.72731f,70.72201f,70.71673f,70.711464f,70.706215f,70.70099f,70.695786f,70.6906f,70.685425f,70.680275f,70.67514f,70.67002f,70.664925f,70.65984f,70.65478f,70.649734f,70.64471f,70.639694f,70.634705f,70.62972f,70.62476f,70.61982f,70.6149f,70.609985f,70.605095f,70.60021f,70.59535f,70.59051f,70.585686f,70.58087f,70.57607f,70.5713f,70.56653f,70.561775f,70.557045f,70.55233f,70.54762f,70.54294f,70.53826f,70.53361f,70.52896f,70.52434f,70.51972f,70.51512f,70.51054f,70.50597f,70.50141f,70.49687f,70.49235f,70.48784f,70.48334f,70.47885f,70.47438f,70.469925f,70.465485f,70.46105f,70.45664f,70.45224f,70.44785f,70.443474f,70.43911f,70.43476f,70.43043f,70.42611f,70.4218f,70.4175f,70.413216f,70.40894f,70.404686f,70.400444f,70.39621f,70.39199f,70.38778f,70.38358f,70.3794f,70.37523f,70.37107f,70.36692f,70.362785f,70.358665f,70.35455f,70.35045f,70.34637f,70.342285f,70.33823f,70.33417f,70.33013f,70.326096f,70.32208f,70.31807f,70.31407f,70.31009f,70.306114f,70.302155f,70.2982f,70.29426f,70.29033f,70.28641f,70.2825f,70.2786f,70.27472f,70.27084f,70.266975f,70.26312f,70.25928f,70.25544f,70.25162f,70.2478f,70.243996f,70.240204f,70.23642f,70.23265f,70.22888f,70.22513f,70.22139f,70.21765f,70.21393f,70.21021f,70.20651f,70.20282f,70.199135f,70.19546f,70.19179f,70.18813f,70.18449f,70.18085f,70.17722f,70.1736f,70.16999f,70.16639f,70.1628f,70.15922f,70.15565f,70.152084f,70.14853f,70.14498f,70.14144f,70.13792f,70.1344f,70.13088f,70.12738f,70.12389f,70.12041f,70.11693f,70.113464f,70.11f,70.10655f,70.10311f,70.09968f,70.09625f,70.092834f,70.089424f,70.08603f,70.082634f,70.07925f,70.075874f,70.07251f,70.069145f,70.065796f,70.062454f,70.05911f,70.055786f,70.05247f,70.04916f,70.04585f,70.04256f,70.03927f,70.03599f,70.032715f,70.02945f,70.02619f,70.02294f,70.0197f,70.01646f,70.01323f,70.01001f,70.0068f,70.00359f,70.0004f,69.9972f,69.99402f,69.990845f,69.98767f,69.98451f,69.98135f,69.97821f,69.97507f,69.97193f,69.9688f,69.96568f,69.96257f,69.959465f,69.95637f,69.95328f,69.950195f,69.94711f,69.94404f,69.94098f,69.93792f,69.93487f,69.93182f,69.92879f,69.92575f,69.92273f,69.91971f,69.916695f,69.91369f,69.91069f,69.9077f,69.90472f,69.90173f,69.898766f,69.8958f,69.89284f,69.88988f,69.88693f,69.88399f,69.88106f,69.87813f,69.875206f,69.872284f,69.86938f,69.86647f,69.86357f,69.86068f,69.857796f,69.85491f,69.852036f,69.84917f,69.846306f,69.84345f,69.8406f,69.83775f,69.834915f,69.83208f,69.829254f,69.82643f,69.823616f,69.8208f,69.818f,69.8152f,69.81241f,69.809616f,69.80684f,69.80406f,69.80129f,69.79852f,69.79576f,69.79301f,69.79026f,69.78751f,69.78478f,69.78204f,69.77932f,69.776596f,69.77388f,69.77117f,69.76846f,69.76576f,69.76307f,69.760376f,69.75769f,69.75501f,69.75234f,69.74967f,69.74701f,69.74435f,69.74169f,69.739044f,69.736404f,69.733765f,69.73113f,69.7285f,69.725876f,69.72326f,69.72064f,69.71803f,69.71543f,69.71284f,69.71024f,69.70765f,69.70507f,69.702484f,69.69991f,69.69734f,69.69478f,69.69222f,69.68967f,69.68711f,69.68457f,69.68203f,69.67949f,69.676956f,69.67443f,69.671906f,69.66939f,69.66688f,69.66437f,69.661865f,69.65936f,69.65687f,69.65438f,69.65189f,69.649414f,69.646935f,69.64446f,69.642f,69.639534f,69.63707f,69.63462f,69.632164f,69.62972f,69.62728f,69.62484f,69.62241f,69.61998f,69.61755f,69.615135f,69.612724f,69.610306f,69.6079f,69.6055f,69.6031f,69.60071f,69.59831f,69.59593f,69.593544f,69.59117f,69.58879f,69.586426f,69.58406f,69.581696f,69.57934f,69.57699f,69.57464f,69.57229f,69.56995f,69.56761f,69.56528f,69.56295f,69.56062f,69.558304f,69.555984f,69.55367f,69.55136f,69.54906f,69.54675f,69.54446f,69.54216f,69.53987f,69.53758f,69.5353f,69.53302f,69.53075f,69.52848f,69.52621f,69.52395f,69.52168f,69.51943f,69.51717f,69.51493f,69.51268f,69.510445f,69.5082f,69.50597f,69.50374f,69.50151f,69.49929f,69.49707f,69.49485f,69.49264f,69.49043f,69.48823f,69.48602f,69.483826f,69.481636f,69.47944f,69.47726f,69.47507f,69.47289f,69.47071f,69.46854f,69.46637f,69.4642f,69.462036f,69.45988f,69.457726f,69.455574f,69.45342f,69.45128f,69.449135f,69.44699f,69.444855f,69.442726f,69.4406f,69.43847f,69.43635f,69.43423f,69.43211f,69.43f,69.42789f,69.42578f,69.42368f,69.42158f,69.41949f,69.41739f,69.4153f,69.413216f,69.41113f,69.40905f,69.406975f,69.4049f,69.40283f,69.40076f,69.3987f,69.39664f,69.39458f,69.392525f,69.39047f,69.38842f,69.386375f,69.38433f,69.38229f,69.38026f,69.37822f,69.37619f,69.37416f,69.37214f,69.37012f,69.368095f,69.36608f,69.364075f,69.36206f,69.360054f,69.358055f,69.35605f,69.35405f,69.35206f,69.35007f,69.348076f,69.34609f,69.34411f,69.34213f,69.34015f,69.33818f,69.336205f,69.334236f,69.332275f,69.33031f,69.32835f,69.32639f,69.32444f,69.32249f,69.32053f,69.31859f,69.31665f,69.314705f,69.31277f,69.31084f,69.30891f,69.30698f,69.305046f,69.30312f,69.3012f,69.299286f,69.29737f,69.295456f,69.29355f,69.29164f,69.289734f,69.287834f,69.285934f,69.284035f,69.28214f,69.28025f,69.27836f,69.276474f,69.27459f,69.27271f,69.27083f,69.26896f,69.26708f,69.26521f,69.26334f,69.26148f,69.25961f,69.25776f,69.2559f,69.25404f,69.25219f,69.25034f,69.24849f,69.24665f,69.244804f,69.242966f,69.24113f,69.239296f,69.23746f,69.235634f,69.2338f,69.23198f,69.230156f,69.22833f,69.22652f,69.2247f,69.22289f,69.22108f,69.21927f,69.21747f,69.21567f,69.21387f,69.21207f,69.210266f,69.20847f,69.20669f,69.204895f,69.20311f,69.201324f,69.19955f,69.19777f,69.19599f,69.194214f,69.192444f,69.190674f,69.1889f,69.18714f,69.18538f,69.18362f,69.181854f,69.1801f,69.178345f,69.1766f,69.17485f,69.1731f,69.17136f,69.16962f,69.16788f,69.16614f,69.1644f,69.16267f,69.160934f,69.15921f,69.15748f,69.155754f,69.15403f,69.15231f,69.1506f,69.14888f,69.14716f,69.145454f,69.143745f,69.14204f,69.140335f,69.13863f,69.13693f,69.13523f,69.13354f,69.13184f,69.13015f,69.12846f,69.12677f,69.12509f,69.123405f,69.12173f,69.12004f,69.11837f,69.11669f,69.11502f,69.11335f,69.11168f,69.110016f,69.108345f,69.10668f,69.10503f,69.10336f,69.10171f,69.10006f,69.098404f,69.096756f,69.09511f,69.09346f,69.09181f,69.09017f,69.08853f,69.08689f,69.08526f,69.083626f,69.08199f,69.08036f,69.078735f,69.0771f,69.075485f,69.07386f,69.07224f,69.07062f,69.06901f,69.06739f,69.06578f,69.06416f,69.06256f,69.06095f,69.05935f,69.05775f,69.056145f,69.05454f,69.05295f,69.05135f,69.04976f,69.048164f,69.04658f,69.04499f,69.0434f,69.04182f,69.04024f,69.03866f,69.03708f,69.0355f,69.03393f,69.03236f,69.030785f,69.02921f,69.02765f,69.02608f,69.02452f,69.02296f,69.02139f,69.01984f,69.01828f,69.01672f,69.015175f,69.013626f,69.01208f,69.01053f,69.00898f,69.00744f,69.0059f,69.00436f,69.002815f,69.00128f,68.99975f,68.998215f,68.99668f,68.99515f,68.99362f,68.992096f,68.99057f,68.98905f,68.987526f,68.98601f,68.98449f,68.98298f,68.98146f,68.97995f,68.97844f,68.97693f,68.975426f,68.973915f,68.97241f,68.97091f,68.96941f,68.96791f,68.966415f,68.96492f,68.963425f,68.96194f,68.96044f,68.958954f,68.957466f,68.95598f,68.9545f,68.95302f,68.95154f,68.95006f,68.94858f,68.947105f,68.94563f,68.94416f,68.94269f,68.941216f,68.93975f,68.938286f,68.93682f,68.935356f,68.9339f,68.932434f,68.93098f,68.92952f,68.92807f,68.92661f,68.92516f,68.92371f,68.922264f,68.920815f,68.91937f,68.91793f,68.91649f,68.91505f,68.913605f,68.91217f,68.910736f,68.9093f,68.90787f,68.90643f,68.90501f,68.90358f,68.90215f,68.90073f,68.8993f,68.89788f,68.89646f,68.89504f,68.89362f,68.892204f,68.89079f,68.88938f,68.88797f,68.88656f,68.88515f,68.88374f,68.88234f,68.880936f,68.87953f,68.87813f,68.87673f,68.87533f,68.87393f,68.872536f,68.87115f,68.86975f,68.86836f,68.866974f,68.865585f,68.8642f,68.862816f,68.86143f,68.86005f,68.858665f,68.857285f,68.85591f,68.85453f,68.85316f,68.85178f,68.85041f,68.84904f,68.84767f,68.846306f,68.84493f,68.84357f,68.84221f,68.84084f,68.839485f,68.83813f,68.83676f,68.83541f,68.83405f,68.832695f,68.831345f,68.829994f,68.82864f,68.82729f,68.82595f,68.8246f,68.82326f,68.821915f,68.82057f,68.81923f,68.817894f,68.81656f,68.815216f,68.81388f,68.81255f,68.81122f,68.80989f,68.808556f,68.80723f,68.8059f,68.80457f,68.80325f,68.801926f,68.800606f,68.799286f,68.797966f,68.796646f,68.795334f,68.794014f,68.7927f,68.79139f,68.79008f,68.78877f,68.78746f,68.786156f,68.78484f,68.78354f};
class mydspSIG2 {
	
  private:
	
	int fmydspSIG2Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG2() {
		return 0;
	}
	int getNumOutputsmydspSIG2() {
		return 1;
	}
	
	void instanceInitmydspSIG2(int sample_rate) {
		fmydspSIG2Wave0_idx = 0;
	}
	
	void fillmydspSIG2(int count, float* table) {
		for (int i3 = 0; i3 < count; i3 = i3 + 1) {
			table[i3] = fmydspSIG2Wave0[fmydspSIG2Wave0_idx];
			fmydspSIG2Wave0_idx = (1 + fmydspSIG2Wave0_idx) % 2001;
		}
	}

};

static mydspSIG2* newmydspSIG2() { return (mydspSIG2*)new mydspSIG2(); }
static void deletemydspSIG2(mydspSIG2* dsp) { delete dsp; }

const static float fmydspSIG3Wave0[2001] = {246.23625f,246.22417f,246.21202f,246.19981f,246.18753f,246.17517f,246.16275f,246.15027f,246.13771f,246.12508f,246.11237f,246.0996f,246.08676f,246.07384f,246.06085f,246.04779f,246.03465f,246.02145f,246.00816f,245.99481f,245.98138f,245.96788f,245.9543f,245.94064f,245.92691f,245.9131f,245.8992f,245.88524f,245.8712f,245.85707f,245.84286f,245.82858f,245.81422f,245.79979f,245.78526f,245.77066f,245.75597f,245.7412f,245.72635f,245.71141f,245.6964f,245.68129f,245.66609f,245.65082f,245.63547f,245.62003f,245.60449f,245.58887f,245.57315f,245.55736f,245.54147f,245.5255f,245.50945f,245.49329f,245.47704f,245.46071f,245.44427f,245.42775f,245.41113f,245.39444f,245.37762f,245.36073f,245.34375f,245.32666f,245.30948f,245.2922f,245.27483f,245.25735f,245.23978f,245.2221f,245.20433f,245.18646f,245.1685f,245.15042f,245.13225f,245.11398f,245.0956f,245.07712f,245.05853f,245.03984f,245.02104f,245.00214f,244.98314f,244.96402f,244.9448f,244.92546f,244.90602f,244.88647f,244.86682f,244.84705f,244.82716f,244.80717f,244.78708f,244.76685f,244.74652f,244.72607f,244.70552f,244.68483f,244.66405f,244.64313f,244.6221f,244.60095f,244.5797f,244.5583f,244.5368f,244.51518f,244.49342f,244.47156f,244.44957f,244.42744f,244.40521f,244.38284f,244.36035f,244.33772f,244.31499f,244.29211f,244.2691f,244.24597f,244.22272f,244.19933f,244.1758f,244.15215f,244.12836f,244.10443f,244.08037f,244.05618f,244.03185f,244.00739f,243.98279f,243.95804f,243.93317f,243.90814f,243.88298f,243.85768f,243.83224f,243.80666f,243.78093f,243.75505f,243.72903f,243.70287f,243.67656f,243.6501f,243.62349f,243.59674f,243.56982f,243.54277f,243.51556f,243.48819f,243.46068f,243.43301f,243.40518f,243.37721f,243.34908f,243.32077f,243.29233f,243.2637f,243.23494f,243.20601f,243.17691f,243.14764f,243.11823f,243.08864f,243.05888f,243.02898f,242.99889f,242.96863f,242.9382f,242.90761f,242.87685f,242.84592f,242.8148f,242.78352f,242.75208f,242.72044f,242.68863f,242.65665f,242.62448f,242.59213f,242.55962f,242.5269f,242.49402f,242.46094f,242.42769f,242.39426f,242.36063f,242.32681f,242.2928f,242.25862f,242.22423f,242.18967f,242.15489f,242.11993f,242.0848f,242.04944f,242.0139f,241.97815f,241.94221f,241.90607f,241.86974f,241.83319f,241.79645f,241.7595f,241.72235f,241.68498f,241.64742f,241.60965f,241.57167f,241.53348f,241.49507f,241.45645f,241.41762f,241.37857f,241.33931f,241.29984f,241.26015f,241.22023f,241.18008f,241.13974f,241.09915f,241.05835f,241.01732f,240.97607f,240.93459f,240.89287f,240.85094f,240.80876f,240.76636f,240.72372f,240.68085f,240.63774f,240.5944f,240.55083f,240.507f,240.46294f,240.41864f,240.3741f,240.3293f,240.28427f,240.23898f,240.19345f,240.14767f,240.10164f,240.05536f,240.00883f,239.96204f,239.915f,239.86769f,239.82013f,239.77231f,239.72423f,239.67589f,239.62729f,239.57841f,239.52927f,239.47987f,239.43019f,239.38025f,239.33003f,239.27954f,239.22879f,239.17775f,239.12643f,239.07484f,239.02296f,238.97081f,238.91838f,238.86566f,238.81265f,238.75935f,238.70578f,238.6519f,238.59775f,238.54329f,238.48854f,238.4335f,238.37816f,238.32253f,238.26659f,238.21036f,238.15381f,238.09697f,238.03983f,237.98236f,237.92459f,237.86652f,237.80814f,237.74944f,237.69043f,237.63109f,237.57144f,237.51147f,237.45119f,237.39058f,237.32965f,237.26839f,237.20682f,237.1449f,237.08266f,237.02008f,236.95718f,236.89394f,236.83037f,236.76645f,236.7022f,236.6376f,236.57266f,236.50739f,236.44176f,236.37578f,236.30946f,236.2428f,236.17577f,236.10838f,236.04065f,235.97256f,235.90411f,235.83531f,235.76614f,235.69661f,235.62671f,235.55644f,235.48581f,235.4148f,235.34343f,235.27167f,235.19955f,235.12704f,235.05417f,234.98091f,234.90726f,234.83324f,234.75882f,234.68402f,234.60884f,234.53326f,234.45729f,234.38092f,234.30415f,234.227f,234.14944f,234.07149f,233.99312f,233.91435f,233.83519f,233.7556f,233.67561f,233.59521f,233.51439f,233.43317f,233.35152f,233.26945f,233.18697f,233.10405f,233.02072f,232.93697f,232.85277f,232.76816f,232.6831f,232.59763f,232.5117f,232.42535f,232.33856f,232.25133f,232.16365f,232.07553f,231.98697f,231.89796f,231.8085f,231.71858f,231.62822f,231.53738f,231.4461f,231.35437f,231.26218f,231.16951f,231.07639f,230.98279f,230.88873f,230.79419f,230.69919f,230.60371f,230.50777f,230.41133f,230.31442f,230.21703f,230.11916f,230.02078f,229.92194f,229.8226f,229.72278f,229.62245f,229.52164f,229.42033f,229.31851f,229.2162f,229.11339f,229.01007f,228.90625f,228.80193f,228.69708f,228.59174f,228.48587f,228.37949f,228.27258f,228.16516f,228.05722f,227.94876f,227.83977f,227.73026f,227.62021f,227.50963f,227.39853f,227.28688f,227.1747f,227.06198f,226.94873f,226.83493f,226.72058f,226.6057f,226.49025f,226.37427f,226.25772f,226.14062f,226.02298f,225.90477f,225.78601f,225.66667f,225.54678f,225.42633f,225.30531f,225.18372f,225.06155f,224.93881f,224.8155f,224.69162f,224.56715f,224.44212f,224.31648f,224.19028f,224.06349f,223.9361f,223.80814f,223.67957f,223.55042f,223.42065f,223.29031f,223.15936f,223.02782f,222.89566f,222.76291f,222.62956f,222.49559f,222.36101f,222.22583f,222.09003f,221.95361f,221.81657f,221.67892f,221.54065f,221.40176f,221.26224f,221.1221f,220.98132f,220.83994f,220.6979f,220.55524f,220.41194f,220.268f,220.12344f,219.97823f,219.83238f,219.68588f,219.53874f,219.39096f,219.24252f,219.09344f,218.94371f,218.79333f,218.64229f,218.4906f,218.33824f,218.18523f,218.03156f,217.87721f,217.72221f,217.56654f,217.41022f,217.2532f,217.09554f,216.9372f,216.77817f,216.61848f,216.45811f,216.29706f,216.13533f,215.97292f,215.80983f,215.64606f,215.4816f,215.31645f,215.15062f,214.98409f,214.81686f,214.64896f,214.48036f,214.31107f,214.14107f,213.97038f,213.79898f,213.62689f,213.4541f,213.28061f,213.1064f,212.9315f,212.75589f,212.57956f,212.40253f,212.2248f,212.04634f,211.86717f,211.68729f,211.5067f,211.3254f,211.14336f,210.96062f,210.77715f,210.59296f,210.40805f,210.22243f,210.03607f,209.84898f,209.66118f,209.47266f,209.2834f,209.09341f,208.9027f,208.71124f,208.51906f,208.32616f,208.1325f,207.93814f,207.74303f,207.54718f,207.3506f,207.15327f,206.95522f,206.75642f,206.55688f,206.35661f,206.15561f,205.95384f,205.75136f,205.54811f,205.34413f,205.1394f,204.93394f,204.72774f,204.52078f,204.31308f,204.10463f,203.89543f,203.6855f,203.47481f,203.26338f,203.0512f,202.83827f,202.62459f,202.41017f,202.19499f,201.97906f,201.7624f,201.54498f,201.32681f,201.1079f,200.88823f,200.6678f,200.44664f,200.22472f,200.00206f,199.77864f,199.55447f,199.32956f,199.1039f,198.87747f,198.65031f,198.42241f,198.19374f,197.96432f,197.73418f,197.50327f,197.27162f,197.03922f,196.80606f,196.57217f,196.33752f,196.10214f,195.86601f,195.62914f,195.39151f,195.15315f,194.91405f,194.6742f,194.4336f,194.19226f,193.9502f,193.70738f,193.46382f,193.21953f,192.9745f,192.72873f,192.48222f,192.23499f,191.98701f,191.73831f,191.48888f,191.23871f,190.98781f,190.73618f,190.48383f,190.23074f,189.97693f,189.7224f,189.46713f,189.21115f,188.95445f,188.69704f,188.4389f,188.18004f,187.92047f,187.66019f,187.39919f,187.13747f,186.87506f,186.61192f,186.34808f,186.08354f,185.8183f,185.55235f,185.2857f,185.01837f,184.75032f,184.48158f,184.21216f,183.94203f,183.67123f,183.39973f,183.12755f,182.85468f,182.58113f,182.3069f,182.032f,181.75641f,181.48015f,181.20323f,180.92563f,180.64737f,180.36844f,180.08885f,179.8086f,179.5277f,179.24612f,178.96391f,178.68105f,178.39752f,178.11336f,177.82855f,177.5431f,177.25703f,176.9703f,176.68295f,176.39497f,176.10637f,175.81714f,175.52728f,175.2368f,174.94571f,174.654f,174.3617f,174.06877f,173.77524f,173.48111f,173.18639f,172.89107f,172.59515f,172.29865f,172.00156f,171.70387f,171.40562f,171.10678f,170.80739f,170.5074f,170.20686f,169.90576f,169.6041f,169.30186f,168.99908f,168.69577f,168.39189f,168.08748f,167.78252f,167.47704f,167.171f,166.86446f,166.55737f,166.24977f,165.94167f,165.63304f,165.3239f,165.01425f,164.7041f,164.39346f,164.08232f,163.77069f,163.45857f,163.14598f,162.8329f,162.51935f,162.20532f,161.89084f,161.57588f,161.26048f,160.94461f,160.62831f,160.31155f,159.99435f,159.67673f,159.35866f,159.04016f,158.72125f,158.40192f,158.08217f,157.76201f,157.44144f,157.12047f,156.79912f,156.47736f,156.15523f,155.8327f,155.5098f,155.18652f,154.86288f,154.53888f,154.21452f,153.88982f,153.56476f,153.23935f,152.9136f,152.58752f,152.26112f,151.93439f,151.60735f,151.27998f,150.95233f,150.62436f,150.2961f,149.96753f,149.63869f,149.30957f,148.98018f,148.6505f,148.32057f,147.99039f,147.65994f,147.32924f,146.9983f,146.66714f,146.33574f,146.00412f,145.67227f,145.34023f,145.00797f,144.6755f,144.34285f,144.01001f,143.67699f,143.34378f,143.0104f,142.67688f,142.3432f,142.00935f,141.67538f,141.34126f,141.00702f,140.67265f,140.33817f,140.00359f,139.66888f,139.3341f,138.99922f,138.66428f,138.32925f,137.99416f,137.65901f,137.3238f,136.98857f,136.6533f,136.31801f,135.98268f,135.64735f,135.31203f,134.97672f,134.64142f,134.30614f,133.97089f,133.6357f,133.30054f,132.96545f,132.63043f,132.2955f,131.96065f,131.6259f,131.29128f,130.95676f,130.62238f,130.28813f,129.95404f,129.6201f,129.28635f,128.95279f,128.61942f,128.28625f,127.953316f,127.620605f,127.28815f,126.95595f,126.624016f,126.29237f,125.96103f,125.63f,125.29929f,124.968925f,124.638916f,124.30927f,123.98002f,123.65117f,123.32273f,122.99473f,122.66717f,122.34008f,122.01347f,121.68736f,121.36178f,121.03672f,120.71222f,120.38829f,120.06496f,119.74223f,119.420135f,119.098694f,118.777916f,118.45784f,118.138466f,117.81983f,117.50195f,117.18485f,116.86855f,116.55307f,116.23844f,115.924675f,115.61181f,115.29985f,114.98884f,114.678795f,114.36974f,114.0617f,113.7547f,113.44877f,113.14393f,112.8402f,112.53763f,112.23621f,111.936005f,111.63702f,111.33927f,111.04281f,110.74764f,110.45381f,110.16133f,109.87023f,109.58054f,109.29229f,109.00549f,108.720184f,108.43639f,108.15413f,107.87343f,107.59432f,107.31682f,107.040955f,106.76675f,106.49422f,106.2234f,105.9543f,105.68695f,105.421364f,105.15757f,104.895584f,104.63542f,104.377106f,104.12064f,103.866066f,103.61338f,103.362595f,103.11374f,102.86681f,102.62184f,102.37882f,102.13777f,101.8987f,101.66161f,101.42652f,101.19343f,100.96234f,100.73326f,100.5062f,100.28116f,100.058136f,99.83713f,99.61814f,99.40118f,99.186226f,98.9733f,98.762375f,98.55346f,98.34655f,98.14164f,97.93871f,97.73777f,97.5388f,97.341805f,97.14676f,96.95366f,96.7625f,96.57326f,96.38593f,96.2005f,96.01697f,95.8353f,95.655495f,95.47754f,95.30141f,95.1271f,94.95459f,94.78386f,94.61491f,94.4477f,94.282234f,94.11849f,93.95644f,93.79609f,93.6374f,93.48036f,93.32496f,93.171165f,93.018974f,92.86837f,92.71932f,92.571815f,92.42584f,92.28138f,92.138405f,91.9969f,91.85686f,91.71825f,91.58106f,91.445274f,91.310875f,91.17785f,91.046165f,90.91582f,90.78679f,90.65906f,90.53261f,90.407425f,90.28349f,90.16079f,90.03931f,89.91902f,89.79993f,89.682f,89.56522f,89.449585f,89.33507f,89.22166f,89.109344f,88.99811f,88.88793f,88.77881f,88.67072f,88.56365f,88.45759f,88.35252f,88.248436f,88.14532f,88.043144f,87.94192f,87.84162f,87.74224f,87.64375f,87.546165f,87.449455f,87.353615f,87.25862f,87.164474f,87.07117f,86.978676f,86.88699f,86.79611f,86.70602f,86.6167f,86.52815f,86.44037f,86.353325f,86.26702f,86.18144f,86.09658f,86.012436f,85.928986f,85.84622f,85.764145f,85.68274f,85.602f,85.52191f,85.44247f,85.36367f,85.2855f,85.20795f,85.13101f,85.05468f,84.97895f,84.90381f,84.829254f,84.75528f,84.68186f,84.60902f,84.53672f,84.46497f,84.39377f,84.3231f,84.25296f,84.183334f,84.11423f,84.04563f,83.97754f,83.909935f,83.84283f,83.77621f,83.71006f,83.64439f,83.579185f,83.51444f,83.45016f,83.38632f,83.32293f,83.25998f,83.197464f,83.13538f,83.07372f,83.01249f,82.95166f,82.89125f,82.831245f,82.771645f,82.71243f,82.65362f,82.59519f,82.53715f,82.479485f,82.422195f,82.36528f,82.30873f,82.25254f,82.19671f,82.141235f,82.08611f,82.03134f,81.976906f,81.92281f,81.86906f,81.815636f,81.76254f,81.70977f,81.657326f,81.605194f,81.55338f,81.501884f,81.45069f,81.3998f,81.34922f,81.298935f,81.24895f,81.19925f,81.14984f,81.10072f,81.051895f,81.00334f,80.95506f,80.90707f,80.859344f,80.81189f,80.7647f,80.71778f,80.67112f,80.624725f,80.57858f,80.53269f,80.48706f,80.44167f,80.39654f,80.35165f,80.307f,80.26259f,80.218414f,80.174484f,80.13078f,80.08731f,80.044075f,80.00106f,79.958275f,79.91571f,79.87337f,79.83124f,79.78933f,79.747635f,79.70616f,79.664894f,79.62383f,79.58298f,79.542336f,79.50189f,79.46165f,79.42161f,79.38176f,79.34212f,79.302666f,79.263405f,79.224335f,79.18546f,79.14677f,79.10827f,79.06995f,79.031815f,78.99386f,78.956085f,78.91849f,78.88107f,78.843834f,78.80676f,78.76987f,78.73314f,78.69659f,78.6602f,78.623985f,78.58793f,78.55204f,78.51631f,78.48075f,78.44534f,78.410095f,78.37501f,78.34008f,78.305305f,78.27068f,78.236206f,78.20189f,78.167725f,78.133705f,78.09983f,78.06611f,78.03253f,77.99909f,77.965805f,77.932655f,77.89965f,77.86678f,77.83405f,77.80146f,77.769005f,77.73669f,77.704506f,77.672455f,77.64054f,77.60876f,77.5771f,77.54558f,77.51418f,77.48291f,77.451775f,77.42075f,77.38987f,77.3591f,77.32846f,77.297935f,77.26754f,77.23726f,77.2071f,77.17706f,77.14714f,77.11734f,77.087654f,77.058075f,77.02862f,76.999275f,76.97005f,76.940926f,76.91192f,76.883026f,76.85423f,76.82556f,76.79699f,76.76853f,76.74017f,76.71193f,76.683784f,76.65575f,76.62781f,76.59998f,76.57225f,76.544624f,76.517105f,76.48968f,76.46235f,76.43513f,76.408f,76.380974f,76.35404f,76.32721f,76.30047f,76.27383f,76.24728f,76.22082f,76.19446f,76.16819f,76.14201f,76.11593f,76.089935f,76.06403f,76.03822f,76.0125f,75.98686f,75.96132f,75.93586f,75.91049f,75.8852f,75.86001f,75.83489f,75.80986f,75.78492f,75.76006f,75.73528f,75.71059f,75.68598f,75.661446f,75.637f,75.61263f,75.58835f,75.56414f,75.540016f,75.51596f,75.492f,75.4681f,75.44429f,75.42055f,75.39689f,75.37331f,75.34979f,75.32636f,75.303f,75.27972f,75.25651f,75.23337f,75.210304f,75.18731f,75.16439f,75.14155f,75.11877f,75.09606f,75.073425f,75.050865f,75.028366f,75.00594f,74.98359f,74.961296f,74.93908f,74.91692f,74.89484f,74.872826f,74.85087f,74.82899f,74.80717f,74.785416f,74.763725f,74.7421f,74.72054f,74.69904f,74.67761f,74.65625f,74.63494f,74.6137f,74.59252f,74.5714f,74.55035f,74.52935f,74.508415f,74.48754f,74.46673f,74.44598f,74.425285f,74.40465f,74.38408f,74.36356f,74.3431f,74.3227f,74.30236f,74.28208f,74.26185f,74.241684f,74.221565f,74.201515f,74.18151f,74.16157f,74.14168f,74.12184f,74.102066f,74.08234f,74.06267f,74.04305f,74.02349f,74.003975f,73.98452f,73.96512f,73.94577f,73.92647f,73.90723f,73.88803f,73.86889f,73.8498f,73.83076f,73.81177f,73.79283f,73.77394f,73.755104f,73.73631f,73.717575f,73.69889f,73.68025f,73.66166f,73.64311f,73.62462f,73.60618f,73.587776f,73.56943f,73.551125f,73.53287f,73.51466f,73.4965f,73.478386f,73.46031f,73.44229f,73.42432f,73.40639f,73.3885f,73.37066f,73.35287f,73.33511f,73.317406f,73.29974f,73.28213f,73.26456f,73.247025f,73.22954f,73.2121f,73.1947f,73.177345f,73.160034f,73.14276f,73.125534f,73.108345f,73.0912f,73.0741f,73.05704f,73.040016f,73.02304f,73.0061f,72.989204f,72.97235f,72.955536f,72.93877f,72.92203f,72.905334f,72.88868f,72.87206f,72.85549f,72.83895f,72.82246f,72.806f,72.78957f,72.77319f,72.75685f,72.74055f,72.72428f,72.70805f,72.691864f,72.67571f,72.65959f,72.64352f,72.62747f,72.611465f,72.595505f,72.57957f,72.563675f,72.54781f,72.532f,72.516205f,72.50046f,72.48474f,72.46906f,72.45342f,72.43781f,72.42224f,72.4067f,72.3912f,72.375725f,72.36029f,72.344894f,72.32952f,72.31419f,72.2989f,72.28363f,72.2684f,72.253204f,72.23804f,72.22291f,72.20781f,72.19274f,72.17771f,72.16271f,72.14774f,72.13281f,72.11791f,72.10304f,72.0882f,72.073395f,72.058624f,72.04388f,72.02917f,72.01449f,71.99984f,71.98522f,71.970634f,71.956085f,71.94156f,71.92706f,71.9126f,71.89817f,71.883766f,71.86939f,71.85505f,71.84074f,71.826454f,71.8122f,71.79798f,71.78378f,71.769615f,71.755486f,71.74138f,71.7273f,71.71325f,71.699234f,71.68524f,71.67128f,71.65734f,71.64344f,71.62956f,71.61571f,71.60189f,71.5881f,71.57433f,71.56059f,71.54688f,71.5332f,71.51955f,71.50591f,71.49232f,71.478745f,71.465195f,71.451675f,71.43818f,71.42471f,71.41128f,71.39786f,71.384476f,71.37111f,71.35778f,71.34447f,71.331184f,71.31793f,71.304695f,71.2915f,71.27831f,71.26516f,71.25203f,71.23893f,71.22585f,71.2128f,71.19977f,71.18677f,71.17379f,71.160835f,71.1479f,71.135f,71.12212f,71.10927f,71.096436f,71.08363f,71.07085f,71.05809f,71.04536f,71.032646f,71.01996f,71.00729f,70.99466f,70.98204f,70.96945f,70.95689f,70.944336f,70.931816f,70.91932f,70.906845f,70.89439f,70.88196f,70.86955f,70.85717f,70.84481f,70.832466f,70.82015f,70.80786f,70.795586f,70.78334f,70.77111f,70.7589f,70.74673f,70.734566f,70.72243f,70.710304f,70.69821f,70.68614f,70.67409f,70.662056f,70.65005f,70.63806f,70.6261f,70.61415f,70.60223f,70.590324f,70.578445f,70.56658f,70.55474f,70.54292f,70.53113f,70.51935f,70.50759f,70.49586f,70.484146f,70.47245f,70.46078f,70.44912f,70.437485f,70.42587f,70.414276f,70.4027f,70.39115f,70.379616f,70.3681f,70.356606f,70.34513f,70.33368f,70.32224f,70.31083f,70.29943f,70.288055f,70.276695f,70.26536f,70.25404f,70.24274f,70.23146f,70.2202f,70.208954f,70.19773f,70.18652f,70.17534f,70.16417f,70.15302f,70.14189f,70.13078f,70.11969f,70.10861f,70.09756f,70.08652f,70.0755f,70.0645f,70.05351f,70.04255f,70.0316f,70.02067f,70.00976f,69.99886f,69.98799f,69.977135f,69.96629f,69.95547f,69.944664f,69.933876f,69.9231f,69.91235f,69.90162f,69.8909f,69.880196f,69.869514f,69.85885f,69.8482f,69.83757f,69.82695f,69.81635f,69.80577f,69.79521f,69.78466f,69.77413f,69.76362f,69.75312f,69.74264f,69.73217f,69.721725f,69.711296f,69.70088f,69.69048f,69.6801f,69.66973f,69.659386f,69.64905f,69.63873f,69.628426f,69.61814f,69.60787f,69.59762f,69.58738f,69.57716f,69.566956f,69.55676f,69.54659f,69.53643f,69.52629f,69.51616f,69.50605f,69.49595f,69.48587f,69.4758f,69.46575f,69.45572f,69.445694f,69.43569f,69.425705f,69.415726f,69.40577f,69.39582f,69.385895f,69.37598f,69.36608f,69.35619f,69.34632f,69.33647f,69.32663f,69.3168f,69.30699f,69.297195f,69.287415f,69.27764f,69.26789f,69.258156f,69.24843f,69.238716f,69.22903f,69.219345f,69.20968f,69.20002f,69.190384f,69.180756f,69.17115f,69.16155f,69.15197f,69.1424f,69.13285f,69.12331f,69.11378f,69.10426f,69.094765f,69.08528f,69.07581f,69.06635f,69.05691f,69.04748f,69.03806f,69.028656f,69.019264f,69.00989f,69.00053f,68.99117f,68.98184f,68.97252f,68.9632f,68.95391f,68.944626f,68.935356f,68.926094f,68.916855f,68.90762f,68.8984f,68.8892f,68.880005f,68.87083f,68.86166f,68.85251f,68.84336f,68.834236f,68.82512f,68.81602f,68.80693f,68.79785f,68.78879f,68.77973f,68.77069f,68.761665f,68.752655f,68.74365f,68.73466f,68.725685f,68.71672f,68.70776f,68.69882f,68.689896f,68.68098f,68.67207f,68.663185f,68.654305f,68.64544f,68.63658f,68.62774f,68.618904f,68.610085f,68.60128f,68.59248f,68.5837f,68.57493f,68.56617f,68.55742f,68.54868f,68.539955f,68.53124f,68.522545f,68.513855f,68.50517f,68.496506f,68.487854f,68.47921f,68.47057f,68.46195f,68.45335f,68.44475f,68.436165f,68.42759f,68.41902f,68.41047f,68.40193f,68.3934f,68.38488f,68.37637f,68.367874f,68.35939f,68.350914f,68.34245f,68.334f,68.32556f,68.31713f,68.30871f,68.3003f,68.2919f,68.283516f,68.27514f,68.26678f,68.25842f,68.25008f,68.241745f,68.23342f,68.22511f,68.21681f,68.20852f,68.20024f,68.19197f,68.183716f,68.17547f,68.16723f,68.159004f,68.15079f,68.14258f,68.134384f,68.1262f,68.11803f,68.10986f,68.10171f,68.09357f,68.085434f,68.07731f,68.0692f,68.06109f,68.053f,68.044914f,68.03684f,68.028786f,68.02073f,68.01269f,68.004654f,67.996635f,67.98862f,67.98062f,67.972626f,67.964645f,67.95667f,67.94871f,67.94076f,67.93281f,67.92488f,67.916954f,67.90904f,67.90114f,67.89324f,67.88536f,67.87748f,67.86961f,67.86176f,67.85391f,67.84608f,67.83825f,67.83043f,67.822624f,67.81483f,67.80704f,67.799255f,67.79149f,67.78372f,67.77597f,67.76823f,67.7605f,67.75277f,67.745056f,67.73735f,67.72966f,67.72197f,67.714294f,67.70663f,67.69897f,67.691315f,67.68367f,67.67604f,67.66842f,67.660805f,67.6532f,67.64561f,67.638016f,67.63044f,67.62287f,67.61531f,67.60776f,67.60022f,67.59268f,67.58516f,67.577644f,67.57014f,67.56264f,67.555145f,67.54767f,67.5402f,67.53273f,67.525276f,67.51783f,67.51039f,67.50297f,67.495544f,67.48814f,67.480736f,67.473335f,67.46595f,67.45857f,67.45121f,67.44385f,67.43649f,67.42915f,67.421814f,67.41449f,67.40717f,67.399864f,67.39256f,67.38527f,67.37798f,67.370705f,67.36344f,67.35618f,67.34893f,67.34168f,67.33445f,67.327225f,67.32f,67.31279f,67.30559f,67.29839f,67.29121f,67.28403f,67.27686f,67.2697f,67.26254f,67.2554f,67.24826f,67.24113f,67.23401f,67.22689f,67.21979f,67.21269f,67.2056f,67.19852f,67.191444f,67.18437f,67.177315f,67.170265f,67.16322f,67.15619f,67.149155f,67.142136f,67.135124f,67.12812f,67.121124f,67.114136f,67.107155f,67.10018f,67.093216f,67.08625f,67.0793f,67.07236f,67.06542f,67.058495f,67.051575f,67.04466f,67.03776f,67.03085f,67.023964f,67.01708f,67.01021f,67.00334f,66.996475f,66.989624f,66.98278f,66.97594f,66.96911f,66.96228f,66.95547f,66.948654f,66.94186f,66.93506f,66.92827f,66.92149f,66.91472f,66.90795f,66.90119f,66.89444f,66.887695f,66.88096f,66.87423f,66.86751f,66.860794f,66.85408f,66.84738f,66.84068f,66.834f,66.82732f,66.82065f,66.81398f,66.80732f,66.80067f,66.79402f,66.787384f,66.780754f,66.774124f,66.76751f,66.7609f,66.754295f,66.747696f,66.74111f,66.73453f,66.72795f,66.72138f,66.71481f,66.70826f,66.70171f};
class mydspSIG3 {
	
  private:
	
	int fmydspSIG3Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG3() {
		return 0;
	}
	int getNumOutputsmydspSIG3() {
		return 1;
	}
	
	void instanceInitmydspSIG3(int sample_rate) {
		fmydspSIG3Wave0_idx = 0;
	}
	
	void fillmydspSIG3(int count, float* table) {
		for (int i4 = 0; i4 < count; i4 = i4 + 1) {
			table[i4] = fmydspSIG3Wave0[fmydspSIG3Wave0_idx];
			fmydspSIG3Wave0_idx = (1 + fmydspSIG3Wave0_idx) % 2001;
		}
	}

};

static mydspSIG3* newmydspSIG3() { return (mydspSIG3*)new mydspSIG3(); }
static void deletemydspSIG3(mydspSIG3* dsp) { delete dsp; }

const static float fmydspSIG4Wave0[2001] = {137.51273f,137.39922f,137.28569f,137.17213f,137.05855f,136.94493f,136.83128f,136.71762f,136.60391f,136.49019f,136.37643f,136.26266f,136.14885f,136.03502f,135.92116f,135.80727f,135.69336f,135.5794f,135.46544f,135.35144f,135.23743f,135.12337f,135.0093f,134.89519f,134.78105f,134.66689f,134.5527f,134.43849f,134.32425f,134.20998f,134.09567f,133.98135f,133.867f,133.75262f,133.63821f,133.52379f,133.40933f,133.29485f,133.18033f,133.06578f,132.95122f,132.83662f,132.722f,132.60735f,132.49268f,132.37798f,132.26324f,132.14848f,132.0337f,131.91888f,131.80405f,131.68918f,131.5743f,131.45937f,131.34442f,131.22945f,131.11446f,130.99942f,130.88437f,130.76929f,130.65419f,130.53905f,130.42389f,130.3087f,130.19348f,130.07823f,129.96297f,129.84767f,129.73235f,129.617f,129.50162f,129.38622f,129.27078f,129.15533f,129.03984f,128.92433f,128.80879f,128.69324f,128.57764f,128.46202f,128.34637f,128.2307f,128.115f,127.999275f,127.88352f,127.767746f,127.65194f,127.5361f,127.42024f,127.30436f,127.188446f,127.0725f,126.956535f,126.84054f,126.72452f,126.608475f,126.4924f,126.3763f,126.26017f,126.14402f,126.02783f,125.91163f,125.795395f,125.67913f,125.56284f,125.446526f,125.330185f,125.21382f,125.09743f,124.981f,124.864555f,124.74808f,124.63158f,124.51505f,124.39849f,124.28191f,124.165306f,124.04867f,123.93201f,123.81532f,123.6986f,123.58186f,123.465096f,123.3483f,123.231476f,123.11463f,122.99776f,122.88085f,122.76392f,122.64697f,122.52999f,122.41298f,122.295944f,122.17889f,122.0618f,121.94468f,121.82754f,121.71037f,121.59318f,121.47595f,121.35871f,121.24143f,121.12413f,121.006805f,120.88945f,120.77207f,120.65466f,120.53723f,120.41977f,120.302284f,120.18477f,120.06722f,119.94966f,119.83206f,119.71445f,119.5968f,119.479126f,119.36143f,119.2437f,119.125946f,119.00816f,118.89036f,118.77252f,118.65466f,118.53678f,118.41887f,118.30093f,118.18296f,118.06497f,117.94695f,117.8289f,117.71083f,117.592735f,117.47461f,117.35645f,117.23827f,117.12007f,117.00184f,116.88358f,116.7653f,116.64699f,116.52865f,116.41028f,116.29189f,116.17347f,116.05503f,115.936554f,115.81806f,115.69953f,115.580986f,115.46241f,115.3438f,115.225174f,115.106514f,114.98783f,114.86912f,114.75038f,114.63162f,114.512825f,114.39401f,114.27517f,114.156296f,114.0374f,113.91848f,113.79953f,113.68055f,113.56155f,113.44252f,113.32346f,113.20438f,113.085266f,112.96613f,112.84697f,112.727776f,112.60856f,112.48932f,112.37005f,112.250755f,112.13143f,112.01208f,111.8927f,111.7733f,111.65387f,111.53442f,111.41493f,111.295425f,111.17589f,111.05632f,110.93674f,110.817116f,110.69747f,110.577805f,110.45811f,110.33839f,110.218636f,110.09886f,109.97906f,109.85922f,109.73937f,109.619484f,109.49957f,109.37964f,109.259674f,109.13969f,109.01967f,108.89963f,108.77956f,108.659454f,108.53933f,108.41918f,108.299f,108.1788f,108.05857f,107.93831f,107.81802f,107.697716f,107.57738f,107.45701f,107.33662f,107.2162f,107.09576f,106.97528f,106.85478f,106.73426f,106.6137f,106.493126f,106.37252f,106.251884f,106.131226f,106.01054f,105.889824f,105.76908f,105.648315f,105.52752f,105.4067f,105.28585f,105.16498f,105.044075f,104.92314f,104.802185f,104.681206f,104.560196f,104.439156f,104.31809f,104.197f,104.07588f,103.95474f,103.833565f,103.71237f,103.59114f,103.46989f,103.34861f,103.2273f,103.10597f,102.98461f,102.86322f,102.741806f,102.62036f,102.49889f,102.377396f,102.255875f,102.13432f,102.01275f,101.89114f,101.769516f,101.64785f,101.52617f,101.40446f,101.282715f,101.16095f,101.039154f,100.917336f,100.79549f,100.67361f,100.551704f,100.42977f,100.307816f,100.18583f,100.06382f,99.94178f,99.81972f,99.697624f,99.5755f,99.453354f,99.33118f,99.20898f,99.08675f,98.964485f,98.8422f,98.71989f,98.59755f,98.47518f,98.35279f,98.23037f,98.10792f,97.98544f,97.86294f,97.74041f,97.61785f,97.49526f,97.37265f,97.250015f,97.12734f,97.00465f,96.88193f,96.75918f,96.6364f,96.51359f,96.390755f,96.2679f,96.14501f,96.022095f,95.89915f,95.77618f,95.65318f,95.53016f,95.407104f,95.28402f,95.16091f,95.03777f,94.91461f,94.79142f,94.6682f,94.54495f,94.42168f,94.29838f,94.17505f,94.05169f,93.92831f,93.80489f,93.68145f,93.55798f,93.43449f,93.31096f,93.18741f,93.06383f,92.940216f,92.81658f,92.69292f,92.56923f,92.44551f,92.32176f,92.19798f,92.07418f,91.95035f,91.82649f,91.70261f,91.57869f,91.45475f,91.33078f,91.20678f,91.08275f,90.958694f,90.83461f,90.7105f,90.586365f,90.4622f,90.338f,90.213776f,90.08952f,89.96524f,89.840935f,89.7166f,89.59223f,89.46784f,89.34342f,89.21897f,89.0945f,88.969986f,88.84545f,88.720894f,88.5963f,88.47168f,88.34704f,88.22236f,88.09766f,87.97292f,87.84816f,87.72337f,87.59856f,87.47371f,87.34884f,87.22393f,87.099f,86.974045f,86.84905f,86.72404f,86.59899f,86.473915f,86.348816f,86.22368f,86.09852f,85.973335f,85.848114f,85.72287f,85.597595f,85.47229f,85.34696f,85.2216f,85.09621f,84.970795f,84.845345f,84.71987f,84.59437f,84.468834f,84.34327f,84.21768f,84.09206f,83.96641f,83.84073f,83.71503f,83.58929f,83.463524f,83.33773f,83.21191f,83.08606f,82.960175f,82.83427f,82.70833f,82.58236f,82.45637f,82.33034f,82.204285f,82.0782f,81.95209f,81.82594f,81.69977f,81.57357f,81.44734f,81.32108f,81.19479f,81.068474f,80.94212f,80.81575f,80.68935f,80.56291f,80.43645f,80.30995f,80.183426f,80.05688f,79.93029f,79.80368f,79.67704f,79.55037f,79.42367f,79.296936f,79.17018f,79.04339f,78.91657f,78.78973f,78.66284f,78.535934f,78.409004f,78.282036f,78.15504f,78.02801f,77.900955f,77.77387f,77.64675f,77.51961f,77.39243f,77.26523f,77.13799f,77.01073f,76.88344f,76.75611f,76.62875f,76.50137f,76.373955f,76.24651f,76.11904f,75.99153f,75.864f,75.736435f,75.60884f,75.48122f,75.35356f,75.225876f,75.09816f,74.97042f,74.84264f,74.714836f,74.587f,74.45914f,74.33124f,74.203316f,74.075356f,73.94737f,73.81935f,73.69131f,73.563225f,73.43512f,73.30698f,73.17881f,73.050606f,72.92238f,72.79412f,72.665825f,72.537506f,72.40915f,72.28077f,72.15236f,72.02391f,71.89544f,71.76693f,71.6384f,71.50983f,71.38123f,71.2526f,71.12395f,70.995255f,70.86653f,70.737785f,70.609f,70.48019f,70.35135f,70.22247f,70.09357f,69.96463f,69.83566f,69.706665f,69.57764f,69.44858f,69.31949f,69.19036f,69.06121f,68.93203f,68.80281f,68.67357f,68.54429f,68.41498f,68.285645f,68.15627f,68.02687f,67.89744f,67.767975f,67.63848f,67.50896f,67.379395f,67.24981f,67.120186f,66.99054f,66.860855f,66.73114f,66.601395f,66.47162f,66.34181f,66.211975f,66.0821f,65.9522f,65.822266f,65.69231f,65.56231f,65.43228f,65.30222f,65.172134f,65.04201f,64.91186f,64.78167f,64.65146f,64.52121f,64.39093f,64.26062f,64.13028f,63.999905f,63.8695f,63.739063f,63.608593f,63.478096f,63.347565f,63.217f,63.086407f,62.95578f,62.825123f,62.69443f,62.56371f,62.432957f,62.302174f,62.171356f,62.040512f,61.90963f,61.77872f,61.647778f,61.516804f,61.385796f,61.25476f,61.12369f,60.992588f,60.861458f,60.730293f,60.5991f,60.46787f,60.33661f,60.20532f,60.073997f,59.942642f,59.811256f,59.67984f,59.54839f,59.41691f,59.285397f,59.15385f,59.022278f,58.89067f,58.75903f,58.62736f,58.49566f,58.363926f,58.23216f,58.10036f,57.968533f,57.836674f,57.704784f,57.57286f,57.440907f,57.30892f,57.176903f,57.044857f,56.912777f,56.780666f,56.64852f,56.51635f,56.384144f,56.251907f,56.11964f,55.987343f,55.85501f,55.722652f,55.59026f,55.457836f,55.325382f,55.192898f,55.06038f,54.927834f,54.795258f,54.662647f,54.530006f,54.39734f,54.264637f,54.131905f,53.999146f,53.866352f,53.73353f,53.600674f,53.467793f,53.334877f,53.201935f,53.06896f,52.935955f,52.80292f,52.669857f,52.536762f,52.403637f,52.270485f,52.1373f,52.004086f,51.870846f,51.73757f,51.60427f,51.470943f,51.33758f,51.204193f,51.070778f,50.937332f,50.803856f,50.670353f,50.536823f,50.403263f,50.269676f,50.13606f,50.002415f,49.868744f,49.735043f,49.60132f,49.467564f,49.333782f,49.199974f,49.06614f,48.932278f,48.79839f,48.664474f,48.530533f,48.39657f,48.262573f,48.128555f,47.994514f,47.860443f,47.72635f,47.59223f,47.45809f,47.323925f,47.18973f,47.05552f,46.92128f,46.787018f,46.652737f,46.51843f,46.3841f,46.24975f,46.115376f,45.980984f,45.84657f,45.71213f,45.57768f,45.443203f,45.308704f,45.17419f,45.039658f,44.905106f,44.770535f,44.63595f,44.501343f,44.366722f,44.232086f,44.09743f,43.962765f,43.828083f,43.693386f,43.558674f,43.42395f,43.289215f,43.15447f,43.019707f,42.884937f,42.75016f,42.61537f,42.480576f,42.345768f,42.210957f,42.07614f,41.94132f,41.806488f,41.671658f,41.536823f,41.401985f,41.267147f,41.13231f,40.997475f,40.86264f,40.727806f,40.59298f,40.458157f,40.32334f,40.18853f,40.05373f,39.91894f,39.78416f,39.649395f,39.51464f,39.379906f,39.245186f,39.110485f,38.975803f,38.841145f,38.706505f,38.571896f,38.43731f,38.302753f,38.16823f,38.033737f,37.899277f,37.76486f,37.630474f,37.49613f,37.36183f,37.22758f,37.093376f,36.95922f,36.82512f,36.691074f,36.557087f,36.423164f,36.289303f,36.155506f,36.021786f,35.888134f,35.754562f,35.62107f,35.48766f,35.35434f,35.22111f,35.087975f,34.95494f,34.822006f,34.68918f,34.556465f,34.423866f,34.29139f,34.159035f,34.026814f,33.894722f,33.762775f,33.630974f,33.49932f,33.367825f,33.23649f,33.105324f,32.97433f,32.843517f,32.71289f,32.582455f,32.45222f,32.32219f,32.192375f,32.06278f,31.93341f,31.804277f,31.675386f,31.546745f,31.418364f,31.290249f,31.162409f,31.034853f,30.907587f,30.780624f,30.653973f,30.527637f,30.401634f,30.275967f,30.150648f,30.025688f,29.901094f,29.77688f,29.653053f,29.529625f,29.406607f,29.284006f,29.161839f,29.040112f,28.918837f,28.798025f,28.677689f,28.557838f,28.438486f,28.319641f,28.201315f,28.08352f,27.966269f,27.849571f,27.733438f,27.617882f,27.502913f,27.388542f,27.274782f,27.161642f,27.049135f,26.93727f,26.826057f,26.715508f,26.605633f,26.49644f,26.387943f,26.28015f,26.173067f,26.066708f,25.961077f,25.856188f,25.752045f,25.648657f,25.546032f,25.44418f,25.343102f,25.24281f,25.143307f,25.044601f,24.946697f,24.8496f,24.753313f,24.657843f,24.563192f,24.469364f,24.376362f,24.284191f,24.19285f,24.10234f,24.012669f,23.92383f,23.83583f,23.748665f,23.662338f,23.576847f,23.492191f,23.40837f,23.325378f,23.24322f,23.161888f,23.081383f,23.001701f,22.922836f,22.84479f,22.767555f,22.691128f,22.615503f,22.540678f,22.466646f,22.393404f,22.320946f,22.249266f,22.178356f,22.108213f,22.038832f,21.970203f,21.90232f,21.83518f,21.768774f,21.703094f,21.638136f,21.57389f,21.510351f,21.44751f,21.38536f,21.323896f,21.26311f,21.202991f,21.143536f,21.084734f,21.026579f,20.969065f,20.912182f,20.855923f,20.800282f,20.745249f,20.690819f,20.636984f,20.583735f,20.531065f,20.478968f,20.427435f,20.37646f,20.326035f,20.276154f,20.226807f,20.17799f,20.129694f,20.081913f,20.03464f,19.987867f,19.94159f,19.895798f,19.850489f,19.805653f,19.761284f,19.717377f,19.673925f,19.63092f,19.58836f,19.546236f,19.504541f,19.46327f,19.422419f,19.381979f,19.341946f,19.302315f,19.263079f,19.224232f,19.185772f,19.14769f,19.109982f,19.072641f,19.035666f,18.999048f,18.962784f,18.926868f,18.891298f,18.856066f,18.821167f,18.7866f,18.752356f,18.718433f,18.684828f,18.651535f,18.618547f,18.585865f,18.553482f,18.521395f,18.489597f,18.458088f,18.426863f,18.395916f,18.365246f,18.334846f,18.304716f,18.27485f,18.245247f,18.215902f,18.18681f,18.15797f,18.129377f,18.101028f,18.072922f,18.045053f,18.01742f,17.990019f,17.962845f,17.935898f,17.909174f,17.882671f,17.856384f,17.830313f,17.804453f,17.778803f,17.753359f,17.728119f,17.70308f,17.67824f,17.653597f,17.629147f,17.604889f,17.580818f,17.556936f,17.53324f,17.509724f,17.486387f,17.46323f,17.440247f,17.417439f,17.3948f,17.372334f,17.350033f,17.327898f,17.305925f,17.284115f,17.262465f,17.24097f,17.219633f,17.19845f,17.17742f,17.156538f,17.135805f,17.115221f,17.094782f,17.074486f,17.05433f,17.034317f,17.014442f,16.994705f,16.975103f,16.955635f,16.9363f,16.917095f,16.898022f,16.879076f,16.860256f,16.841564f,16.822994f,16.804548f,16.786222f,16.768019f,16.749931f,16.731964f,16.714111f,16.696375f,16.678751f,16.661242f,16.643843f,16.626553f,16.609373f,16.592302f,16.575338f,16.55848f,16.541725f,16.525074f,16.508526f,16.49208f,16.475735f,16.459488f,16.44334f,16.42729f,16.411335f,16.395477f,16.379713f,16.364042f,16.348465f,16.33298f,16.317583f,16.302279f,16.287064f,16.271935f,16.256895f,16.241941f,16.227074f,16.21229f,16.197592f,16.182976f,16.168444f,16.153992f,16.139622f,16.12533f,16.11112f,16.096987f,16.082933f,16.068956f,16.055056f,16.041231f,16.027481f,16.013807f,16.000206f,15.986677f,15.973222f,15.959838f,15.946525f,15.933283f,15.92011f,15.907007f,15.893972f,15.881006f,15.868107f,15.855274f,15.842508f,15.829807f,15.817172f,15.804602f,15.792095f,15.779652f,15.767271f,15.754952f,15.742696f,15.730501f,15.718367f,15.706292f,15.694278f,15.682323f,15.670426f,15.658588f,15.646808f,15.635085f,15.623419f,15.611809f,15.600254f,15.588756f,15.577312f,15.565923f,15.554588f,15.543306f,15.532078f,15.520903f,15.50978f,15.498709f,15.487689f,15.47672f,15.465802f,15.454935f,15.444118f,15.43335f,15.42263f,15.411961f,15.401339f,15.390764f,15.380239f,15.36976f,15.359327f,15.348942f,15.338603f,15.328309f,15.318062f,15.307858f,15.297701f,15.287587f,15.277517f,15.267492f,15.25751f,15.247571f,15.237675f,15.227821f,15.21801f,15.20824f,15.198512f,15.188826f,15.179179f,15.169575f,15.160009f,15.150485f,15.141001f,15.131555f,15.122149f,15.112782f,15.103454f,15.094164f,15.084911f,15.075698f,15.066522f,15.057382f,15.04828f,15.039215f,15.030186f,15.0211935f,15.012238f,15.003317f,14.9944315f,14.985582f,14.976768f,14.967988f,14.959243f,14.950532f,14.941855f,14.933212f,14.924603f,14.916027f,14.907485f,14.898975f,14.890498f,14.882054f,14.873642f,14.865263f,14.8569145f,14.8485985f,14.840314f,14.832061f,14.823838f,14.815647f,14.807486f,14.7993555f,14.791256f,14.783186f,14.775146f,14.767136f,14.759154f,14.751203f,14.74328f,14.735387f,14.727523f,14.7196865f,14.711879f,14.7041f,14.696348f,14.688624f,14.680928f,14.673261f,14.665619f,14.658006f,14.650419f,14.642859f,14.635325f,14.627819f,14.620338f,14.6128845f,14.605456f,14.598055f,14.590678f,14.583327f,14.576002f,14.568702f,14.561426f,14.554176f,14.546951f,14.53975f,14.532575f,14.525423f,14.518295f,14.511191f,14.504112f,14.497057f,14.490025f,14.483016f,14.476031f,14.4690695f,14.462131f,14.455215f,14.448322f,14.441452f,14.434605f,14.427779f,14.420977f,14.414196f,14.407438f,14.4007015f,14.393987f,14.387294f,14.380623f,14.373973f,14.367344f,14.360737f,14.354151f,14.347586f,14.341041f,14.3345175f,14.328014f,14.321532f,14.31507f,14.308628f,14.302207f,14.295806f,14.289424f,14.283063f,14.276721f,14.270399f,14.264096f,14.2578125f,14.251549f,14.245304f,14.2390785f,14.232873f,14.226685f,14.220516f,14.214366f,14.208235f,14.202122f,14.196028f,14.189952f,14.183893f,14.177854f,14.171832f,14.165829f,14.1598425f,14.153874f,14.147923f,14.141991f,14.136076f,14.1301775f,14.124297f,14.118433f,14.112587f,14.106758f,14.100945f,14.09515f,14.089371f,14.08361f,14.077864f,14.072135f,14.066422f,14.060726f,14.055047f,14.049383f,14.0437355f,14.038104f,14.032489f,14.026889f,14.021305f,14.015738f,14.010185f,14.004648f,13.999126f,13.993621f,13.988131f,13.982655f,13.977195f,13.97175f,13.96632f,13.960905f,13.955504f,13.95012f,13.944749f,13.939393f,13.934052f,13.928725f,13.923413f,13.918116f,13.912832f,13.907562f,13.9023075f,13.897067f,13.89184f,13.886627f,13.881429f,13.876244f,13.871073f,13.865915f,13.860771f,13.855641f,13.850525f,13.845422f,13.840332f,13.835256f,13.830193f,13.825143f,13.8201065f,13.815083f,13.810072f,13.805075f,13.800091f,13.795118f,13.79016f,13.785214f,13.780281f,13.77536f,13.7704525f,13.765556f,13.7606735f,13.755803f,13.750945f,13.7460985f,13.741265f,13.7364435f,13.731634f,13.726837f,13.722052f,13.7172785f,13.712518f,13.707768f,13.703031f,13.698305f,13.69359f,13.688889f,13.684197f,13.679519f,13.67485f,13.670195f,13.66555f,13.660916f,13.656295f,13.651684f,13.647084f,13.642496f,13.637918f,13.633352f,13.628798f,13.624253f,13.61972f,13.615198f,13.610686f,13.606186f,13.601696f,13.597217f,13.592749f,13.588291f,13.583843f,13.579407f,13.574981f,13.570565f,13.566159f,13.561765f,13.55738f,13.553005f,13.548641f,13.544288f,13.539944f,13.53561f,13.531286f,13.526973f,13.522669f,13.518375f,13.5140915f,13.509818f,13.505553f,13.501299f,13.497055f,13.49282f,13.488595f,13.484379f,13.480173f,13.475977f,13.471789f,13.467612f,13.463445f,13.459286f,13.455136f,13.450996f,13.446866f,13.442744f,13.438632f,13.434529f,13.430435f,13.426351f,13.422276f,13.418208f,13.414151f,13.410102f,13.406062f,13.402032f,13.398009f,13.393996f,13.389992f,13.385997f,13.3820095f,13.378032f,13.374062f,13.370101f,13.366149f,13.362205f,13.35827f,13.354342f,13.350425f,13.346515f,13.342613f,13.33872f,13.334835f,13.330958f,13.32709f,13.323231f,13.319379f,13.315535f,13.3117f,13.307872f,13.304053f,13.300241f,13.296438f,13.292644f,13.2888565f,13.285077f,13.281305f,13.277542f,13.273787f,13.270039f,13.266298f,13.262567f,13.2588415f,13.255125f,13.251415f,13.247714f,13.24402f,13.240334f,13.236655f,13.232984f,13.22932f,13.225664f,13.222015f,13.218373f,13.21474f,13.211113f,13.207494f,13.203882f,13.200277f,13.19668f,13.19309f,13.1895075f,13.185931f,13.182363f,13.178802f,13.175247f,13.1717005f,13.16816f,13.164627f,13.161101f,13.157582f,13.15407f,13.150565f,13.147067f,13.143576f,13.140091f,13.136613f,13.133142f,13.129679f,13.126222f,13.122771f,13.119328f,13.1158905f,13.11246f,13.109036f,13.105619f,13.102209f,13.098804f,13.0954075f,13.092017f,13.088633f,13.085255f,13.081883f,13.078519f,13.075161f,13.071809f,13.068463f,13.0651245f,13.061791f,13.058465f,13.055144f,13.051831f,13.048523f,13.045222f,13.041926f,13.038638f,13.035355f,13.032079f,13.028808f,13.025544f,13.022285f,13.019033f,13.015787f,13.0125475f,13.009313f,13.006085f,13.002863f,12.999647f,12.996437f,12.993233f,12.990034f,12.986841f,12.983655f,12.980474f,12.9773f,12.974131f,12.970967f,12.96781f,12.964658f,12.961512f,12.958371f,12.955236f,12.952107f,12.948984f,12.945867f,12.942755f,12.939648f,12.936547f,12.933452f,12.930362f,12.927278f,12.924199f,12.921125f,12.918057f,12.914995f,12.911939f,12.908887f,12.905841f,12.902801f,12.899765f,12.896735f,12.893711f,12.890692f,12.887678f,12.884669f,12.881666f,12.878668f,12.875675f,12.872687f,12.869705f,12.866728f,12.863756f,12.860789f,12.857827f,12.854871f,12.851919f,12.848973f,12.846032f,12.843096f,12.840165f,12.837238f,12.834317f,12.831402f,12.82849f,12.825584f,12.822683f,12.819787f,12.8168955f,12.81401f,12.811128f,12.808251f,12.80538f,12.802513f,12.799651f,12.796794f,12.7939415f,12.791094f,12.788251f,12.785414f,12.78258f,12.779752f,12.776928f,12.774109f,12.771295f,12.768485f,12.76568f,12.762879f,12.760084f,12.757293f,12.754507f,12.751725f,12.748948f,12.746175f,12.743407f,12.7406435f,12.7378845f,12.73513f,12.73238f,12.729634f,12.726893f,12.724157f,12.721425f,12.718698f,12.715975f,12.713256f,12.710542f,12.707831f,12.705126f,12.702425f,12.699728f,12.697036f,12.694347f,12.691664f,12.688984f,12.686309f,12.683638f,12.680971f,12.6783085f,12.675651f,12.6729965f,12.670347f,12.667702f,12.66506f,12.662423f,12.65979f,12.657162f,12.654537f,12.6519165f,12.649301f,12.646688f,12.64408f,12.641476f,12.638876f,12.63628f,12.633688f,12.631101f,12.628517f,12.625937f,12.623362f,12.62079f,12.618222f,12.615659f,12.613099f,12.610543f,12.607991f,12.605443f,12.6029f,12.600359f,12.597823f,12.595291f,12.592763f,12.590238f,12.587717f,12.5852f,12.582687f,12.580178f,12.577673f,12.575172f,12.572675f,12.570181f,12.567691f,12.565204f,12.562721f,12.560243f,12.557768f,12.555296f,12.552829f,12.5503645f,12.547905f,12.545448f,12.542995f,12.540546f,12.5381f,12.535659f,12.53322f,12.530786f,12.528355f,12.525928f,12.523504f,12.521084f,12.518667f,12.516254f,12.5138445f,12.511439f,12.509037f,12.506638f,12.504243f,12.501851f,12.499463f,12.497078f,12.494697f,12.492319f,12.489944f,12.487574f,12.485207f,12.482842f,12.480482f,12.478125f,12.475771f,12.473421f,12.471074f,12.468731f,12.466391f,12.464054f,12.46172f,12.459391f,12.457065f,12.4547415f,12.452421f,12.450105f,12.447791f,12.445481f,12.443174f,12.440871f,12.438571f,12.436275f,12.433981f,12.43169f,12.429403f,12.427119f,12.424839f,12.422562f,12.420287f,12.418016f,12.415749f,12.413485f,12.411222f,12.408964f,12.40671f,12.404458f,12.402209f,12.399963f,12.397721f,12.395482f,12.393246f,12.391012f,12.3887825f,12.386556f,12.384332f,12.382112f,12.379893f,12.377679f,12.375467f,12.373259f,12.371054f,12.368852f,12.366652f,12.364455f,12.362263f,12.360072f,12.357884f,12.3557005f,12.353519f,12.35134f,12.349165f,12.346993f,12.344824f,12.342657f,12.340493f,12.338333f,12.336175f,12.334021f,12.331868f,12.32972f,12.327574f,12.325431f,12.32329f,12.321153f,12.319018f,12.316887f,12.314757f,12.312632f,12.310509f,12.308389f,12.306271f,12.304156f,12.302045f,12.299935f,12.29783f,12.295726f,12.293626f,12.291528f,12.289433f,12.28734f,12.285251f,12.283164f,12.28108f,12.278999f,12.27692f,12.274845f,12.272772f,12.270701f,12.268634f,12.266569f,12.264506f,12.262447f,12.26039f,12.258336f,12.256285f,12.254236f,12.25219f,12.250147f,12.248106f,12.246067f,12.244032f,12.241999f,12.239968f,12.237941f,12.235916f,12.233893f,12.2318735f,12.2298565f,12.227842f,12.22583f,12.223821f,12.221813f,12.21981f,12.217808f,12.215808f,12.213812f,12.211818f,12.2098255f,12.207837f,12.20585f,12.203866f,12.201884f,12.199905f,12.197929f,12.195955f,12.193984f,12.192015f,12.190048f,12.188084f,12.186123f,12.184163f,12.182207f,12.180252f,12.178301f,12.176352f,12.174404f,12.17246f,12.170518f,12.168578f,12.166641f,12.164706f,12.162774f,12.160844f,12.158916f,12.156991f,12.155068f,12.153148f,12.15123f,12.149314f,12.147401f,12.14549f,12.143581f,12.141675f,12.139771f,12.13787f,12.13597f,12.134073f,12.132178f,12.130286f,12.128396f,12.126509f,12.124623f,12.12274f,12.120859f,12.11898f,12.117105f,12.115231f,12.1133585f,12.111489f,12.109622f,12.107758f,12.105895f,12.104034f,12.102176f,12.10032f,12.098467f,12.096615f,12.094766f,12.092918f,12.091074f,12.0892315f,12.087391f,12.085553f,12.083716f,12.081882f,12.080051f,12.078222f,12.076394f,12.07457f,12.072746f,12.070926f,12.069107f,12.06729f,12.065476f};
class mydspSIG4 {
	
  private:
	
	int fmydspSIG4Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG4() {
		return 0;
	}
	int getNumOutputsmydspSIG4() {
		return 1;
	}
	
	void instanceInitmydspSIG4(int sample_rate) {
		fmydspSIG4Wave0_idx = 0;
	}
	
	void fillmydspSIG4(int count, float* table) {
		for (int i5 = 0; i5 < count; i5 = i5 + 1) {
			table[i5] = fmydspSIG4Wave0[fmydspSIG4Wave0_idx];
			fmydspSIG4Wave0_idx = (1 + fmydspSIG4Wave0_idx) % 2001;
		}
	}

};

static mydspSIG4* newmydspSIG4() { return (mydspSIG4*)new mydspSIG4(); }
static void deletemydspSIG4(mydspSIG4* dsp) { delete dsp; }

const static float fmydspSIG5Wave0[2001] = {214.80658f,214.65059f,214.49445f,214.33815f,214.1817f,214.0251f,213.86835f,213.71146f,213.5544f,213.39722f,213.23987f,213.08238f,212.92476f,212.76698f,212.60907f,212.451f,212.2928f,212.13446f,211.97597f,211.81734f,211.65857f,211.49966f,211.34062f,211.18144f,211.02213f,210.86267f,210.70308f,210.54335f,210.38348f,210.2235f,210.06335f,209.90309f,209.7427f,209.58217f,209.42152f,209.26073f,209.09981f,208.93877f,208.77759f,208.61629f,208.45485f,208.29329f,208.1316f,207.96979f,207.80786f,207.6458f,207.48361f,207.32129f,207.15886f,206.9963f,206.83363f,206.67082f,206.5079f,206.34486f,206.1817f,206.01842f,205.85501f,205.6915f,205.52786f,205.3641f,205.20023f,205.03624f,204.87213f,204.70792f,204.54358f,204.37912f,204.21455f,204.04988f,203.88509f,203.72018f,203.55516f,203.39003f,203.2248f,203.05943f,202.89398f,202.72841f,202.56271f,202.39693f,202.23102f,202.06502f,201.8989f,201.73267f,201.56635f,201.3999f,201.23335f,201.0667f,200.89995f,200.73308f,200.56612f,200.39905f,200.23187f,200.06459f,199.8972f,199.72972f,199.56213f,199.39444f,199.22665f,199.05876f,198.89078f,198.72267f,198.55449f,198.3862f,198.2178f,198.04932f,197.88072f,197.71204f,197.54324f,197.37436f,197.20538f,197.03632f,196.86714f,196.69786f,196.5285f,196.35904f,196.18948f,196.01984f,195.8501f,195.68025f,195.51033f,195.3403f,195.17018f,194.99997f,194.82967f,194.65927f,194.48878f,194.3182f,194.14754f,193.97678f,193.80592f,193.63498f,193.46396f,193.29283f,193.12163f,192.95033f,192.77895f,192.60747f,192.4359f,192.26425f,192.09251f,191.92068f,191.74878f,191.57678f,191.4047f,191.23253f,191.06027f,190.88792f,190.7155f,190.54298f,190.37039f,190.19771f,190.02495f,189.8521f,189.67917f,189.50615f,189.33305f,189.15987f,188.9866f,188.81326f,188.63983f,188.46632f,188.29274f,188.11906f,187.94531f,187.77148f,187.59758f,187.42358f,187.24951f,187.07536f,186.90112f,186.72682f,186.55243f,186.37796f,186.20341f,186.0288f,185.8541f,185.6793f,185.50446f,185.32953f,185.15451f,184.97943f,184.80426f,184.62903f,184.4537f,184.27832f,184.10284f,183.9273f,183.7517f,183.576f,183.40024f,183.2244f,183.04848f,182.87248f,182.69643f,182.5203f,182.34407f,182.1678f,181.99144f,181.81502f,181.6385f,181.46194f,181.2853f,181.10858f,180.9318f,180.75493f,180.578f,180.401f,180.22392f,180.04678f,179.86957f,179.69228f,179.51492f,179.33751f,179.16f,178.98245f,178.80481f,178.6271f,178.44934f,178.2715f,178.0936f,177.91562f,177.73758f,177.55946f,177.38129f,177.20303f,177.02472f,176.84634f,176.6679f,176.48938f,176.31079f,176.13214f,175.95343f,175.77466f,175.59581f,175.41689f,175.23792f,175.05887f,174.87976f,174.70059f,174.52135f,174.34204f,174.16267f,173.98325f,173.80374f,173.62419f,173.44456f,173.26488f,173.08513f,172.9053f,172.72543f,172.54549f,172.36548f,172.18541f,172.00528f,171.82509f,171.64484f,171.46452f,171.28415f,171.1037f,170.9232f,170.74263f,170.56201f,170.38133f,170.20058f,170.01978f,169.8389f,169.65797f,169.47697f,169.29593f,169.11482f,168.93365f,168.75243f,168.57114f,168.38979f,168.20837f,168.0269f,167.84537f,167.66379f,167.48215f,167.30045f,167.11868f,166.93686f,166.75497f,166.57304f,166.39104f,166.20898f,166.02689f,165.84471f,165.66249f,165.4802f,165.29787f,165.11546f,164.93301f,164.75049f,164.56793f,164.3853f,164.20262f,164.01988f,163.8371f,163.65424f,163.47133f,163.28838f,163.10536f,162.92229f,162.73915f,162.55597f,162.37274f,162.18944f,162.00609f,161.8227f,161.63924f,161.45572f,161.27216f,161.08853f,160.90486f,160.72113f,160.53735f,160.35352f,160.16962f,159.98567f,159.80168f,159.61763f,159.43353f,159.24937f,159.06516f,158.88089f,158.69658f,158.5122f,158.32779f,158.14333f,157.9588f,157.77422f,157.58958f,157.4049f,157.22017f,157.03539f,156.85056f,156.66566f,156.48073f,156.29573f,156.11069f,155.9256f,155.74045f,155.55525f,155.37001f,155.18472f,154.99937f,154.81398f,154.62852f,154.44302f,154.25748f,154.07188f,153.88623f,153.70053f,153.51479f,153.329f,153.14314f,152.95724f,152.7713f,152.58531f,152.39928f,152.21318f,152.02704f,151.84085f,151.6546f,151.46832f,151.28198f,151.0956f,150.90916f,150.72269f,150.53616f,150.34958f,150.16296f,149.97629f,149.78957f,149.6028f,149.41599f,149.22913f,149.0422f,148.85526f,148.66826f,148.4812f,148.2941f,148.10696f,147.91977f,147.73253f,147.54524f,147.35791f,147.17053f,146.98311f,146.79564f,146.60812f,146.42056f,146.23296f,146.0453f,145.8576f,145.66985f,145.48206f,145.29422f,145.10634f,144.91841f,144.73044f,144.54242f,144.35437f,144.16626f,143.9781f,143.7899f,143.60167f,143.41338f,143.22505f,143.03667f,142.84825f,142.65979f,142.47128f,142.28273f,142.09413f,141.90549f,141.71681f,141.52808f,141.33931f,141.1505f,140.96164f,140.77274f,140.58379f,140.3948f,140.20576f,140.0167f,139.82758f,139.63841f,139.4492f,139.25996f,139.07068f,138.88135f,138.69197f,138.50255f,138.3131f,138.1236f,137.93405f,137.74446f,137.55484f,137.36517f,137.17546f,136.9857f,136.79591f,136.60608f,136.4162f,136.22627f,136.03632f,135.84631f,135.65628f,135.46619f,135.27606f,135.0859f,134.89569f,134.70544f,134.51515f,134.32483f,134.13446f,133.94405f,133.7536f,133.56311f,133.37257f,133.182f,132.9914f,132.80074f,132.61005f,132.41931f,132.22855f,132.03773f,131.84688f,131.65599f,131.46506f,131.2741f,131.08308f,130.89203f,130.70094f,130.50981f,130.31865f,130.12744f,129.9362f,129.74492f,129.5536f,129.36224f,129.17084f,128.9794f,128.78793f,128.59642f,128.40486f,128.21327f,128.02164f,127.82998f,127.638275f,127.44653f,127.25475f,127.06293f,126.87108f,126.679184f,126.48725f,126.29528f,126.10327f,125.91123f,125.71915f,125.52703f,125.33488f,125.142685f,124.950455f,124.75819f,124.56589f,124.37354f,124.18117f,123.988754f,123.7963f,123.60382f,123.4113f,123.218735f,123.026146f,122.83351f,122.64085f,122.44814f,122.2554f,122.06263f,121.86981f,121.67697f,121.484085f,121.29117f,121.09821f,120.90523f,120.7122f,120.51914f,120.32604f,120.13291f,119.93975f,119.74655f,119.553314f,119.36005f,119.16674f,118.9734f,118.78002f,118.58662f,118.39317f,118.19969f,118.00618f,117.81264f,117.61906f,117.425446f,117.2318f,117.03811f,116.84439f,116.65064f,116.45686f,116.26304f,116.06919f,115.875305f,115.68139f,115.487434f,115.29345f,115.099434f,114.90538f,114.711296f,114.51718f,114.32303f,114.128845f,113.93463f,113.74038f,113.5461f,113.35178f,113.15744f,112.96306f,112.768654f,112.5742f,112.37973f,112.18523f,111.990685f,111.79611f,111.60151f,111.406876f,111.21221f,111.01751f,110.822784f,110.62802f,110.43323f,110.2384f,110.04355f,109.84866f,109.65375f,109.45879f,109.26382f,109.06881f,108.873764f,108.678696f,108.48359f,108.28846f,108.0933f,107.8981f,107.70288f,107.50763f,107.31234f,107.11703f,106.921684f,106.72631f,106.53091f,106.33548f,106.140015f,105.94452f,105.749f,105.55345f,105.35787f,105.16226f,104.96662f,104.77096f,104.57526f,104.37953f,104.183784f,103.988f,103.79219f,103.59635f,103.40048f,103.20458f,103.00866f,102.812706f,102.61673f,102.42072f,102.224686f,102.028625f,101.83253f,101.63641f,101.44027f,101.244095f,101.04789f,100.85166f,100.65541f,100.45913f,100.26282f,100.06648f,99.87012f,99.67373f,99.47732f,99.280876f,99.084404f,98.88791f,98.69139f,98.49484f,98.29827f,98.10167f,97.90505f,97.7084f,97.51173f,97.315025f,97.1183f,96.92155f,96.72478f,96.52798f,96.33115f,96.1343f,95.93742f,95.740524f,95.5436f,95.34666f,95.14968f,94.95269f,94.75567f,94.558624f,94.36156f,94.16447f,93.967354f,93.77022f,93.57305f,93.37587f,93.178665f,92.98144f,92.78419f,92.58691f,92.38961f,92.19229f,91.99495f,91.79759f,91.600204f,91.4028f,91.20537f,91.00792f,90.81045f,90.61296f,90.41544f,90.21791f,90.020355f,89.822784f,89.62518f,89.42757f,89.229935f,89.03228f,88.8346f,88.63691f,88.43919f,88.24146f,88.04371f,87.84594f,87.64815f,87.45034f,87.25251f,87.054665f,86.856804f,86.65892f,86.46102f,86.26311f,86.06517f,85.867226f,85.66926f,85.471275f,85.27327f,85.07526f,84.87722f,84.67918f,84.48111f,84.283035f,84.08494f,83.88683f,83.68871f,83.49057f,83.29242f,83.09425f,82.89608f,82.69789f,82.49968f,82.30147f,82.10323f,81.905f,81.70674f,81.50848f,81.3102f,81.111916f,80.91362f,80.71531f,80.51699f,80.318665f,80.12033f,79.92199f,79.72363f,79.525276f,79.326904f,79.12853f,78.930145f,78.73176f,78.53336f,78.33497f,78.13656f,77.93815f,77.73974f,77.54132f,77.3429f,77.14448f,76.94605f,76.74763f,76.5492f,76.35078f,76.15234f,75.95392f,75.75549f,75.557076f,75.35865f,75.16024f,74.96182f,74.76342f,74.56502f,74.36662f,74.168236f,73.96986f,73.771484f,73.57312f,73.37477f,73.17643f,72.9781f,72.77979f,72.58149f,72.38321f,72.184944f,71.986694f,71.78846f,71.59025f,71.39206f,71.19389f,70.99574f,70.79762f,70.59953f,70.40147f,70.20343f,70.005424f,69.80745f,69.609505f,69.4116f,69.21373f,69.0159f,68.818115f,68.62036f,68.42266f,68.225006f,68.0274f,67.829834f,67.63233f,67.434875f,67.23749f,67.040146f,66.84287f,66.64567f,66.448524f,66.25146f,66.05445f,65.85753f,65.66068f,65.46392f,65.26724f,65.070656f,64.87415f,64.67775f,64.481445f,64.28524f,64.08914f,63.893158f,63.697285f,63.50153f,63.3059f,63.110397f,62.915028f,62.71979f,62.5247f,62.329758f,62.134964f,61.94033f,61.74586f,61.55156f,61.357433f,61.163486f,60.96973f,60.77617f,60.58281f,60.389656f,60.196716f,60.004f,59.811516f,59.619267f,59.427265f,59.235516f,59.04403f,58.852814f,58.66188f,58.471233f,58.280884f,58.090843f,57.901123f,57.711727f,57.52267f,57.33396f,57.14561f,56.957634f,56.770035f,56.582832f,56.396034f,56.209652f,56.0237f,55.838192f,55.653137f,55.468548f,55.284443f,55.10083f,54.91773f,54.735146f,54.553097f,54.3716f,54.190674f,54.01032f,53.830563f,53.651417f,53.472893f,53.29501f,53.11778f,52.94122f,52.76535f,52.59018f,52.415726f,52.242004f,52.069035f,51.896828f,51.725403f,51.55477f,51.384953f,51.215965f,51.047817f,50.880527f,50.71411f,50.548584f,50.38396f,50.220257f,50.057484f,49.895657f,49.73479f,49.5749f,49.415997f,49.258095f,49.101204f,48.94534f,48.79051f,48.63673f,48.48401f,48.33236f,48.18179f,48.032307f,47.883926f,47.73665f,47.59049f,47.445454f,47.301544f,47.15877f,47.017143f,46.87666f,46.737328f,46.59915f,46.46214f,46.326286f,46.1916f,46.058083f,45.92573f,45.79455f,45.664543f,45.535706f,45.408035f,45.281532f,45.1562f,45.03203f,44.90902f,44.78717f,44.666477f,44.546932f,44.42854f,44.311283f,44.195164f,44.08018f,43.96632f,43.853577f,43.741947f,43.631424f,43.521996f,43.413662f,43.306408f,43.200233f,43.09512f,42.99107f,42.888065f,42.786102f,42.685173f,42.585266f,42.48637f,42.38848f,42.291584f,42.19567f,42.10073f,42.006756f,41.91374f,41.821663f,41.730526f,41.64031f,41.55101f,41.462612f,41.37511f,41.28849f,41.202747f,41.117863f,41.033836f,40.950653f,40.8683f,40.78677f,40.706055f,40.626144f,40.547024f,40.46869f,40.39113f,40.31433f,40.238285f,40.162987f,40.088425f,40.014587f,39.941467f,39.869053f,39.797337f,39.72631f,39.655964f,39.58629f,39.517277f,39.448917f,39.381207f,39.31413f,39.24768f,39.181854f,39.11664f,39.052025f,38.98801f,38.924583f,38.861736f,38.79946f,38.73775f,38.676598f,38.615997f,38.555935f,38.496414f,38.43742f,38.378944f,38.320988f,38.26354f,38.20659f,38.150135f,38.09417f,38.03869f,37.98368f,37.929142f,37.875065f,37.82145f,37.768284f,37.71556f,37.66328f,37.611435f,37.560017f,37.50902f,37.458443f,37.40828f,37.35852f,37.309162f,37.260204f,37.211636f,37.163452f,37.115654f,37.06823f,37.02118f,36.974495f,36.928177f,36.882214f,36.83661f,36.79135f,36.746437f,36.701866f,36.657635f,36.61373f,36.57016f,36.526913f,36.483986f,36.441376f,36.399082f,36.357094f,36.315414f,36.274036f,36.232956f,36.19217f,36.151676f,36.11147f,36.07155f,36.03191f,35.99255f,35.953465f,35.91465f,35.876102f,35.837822f,35.799805f,35.762047f,35.724545f,35.687298f,35.6503f,35.61355f,35.577045f,35.540783f,35.50476f,35.468975f,35.433426f,35.39811f,35.363018f,35.328156f,35.293518f,35.2591f,35.224907f,35.190926f,35.15716f,35.12361f,35.090267f,35.057133f,35.024208f,34.99148f,34.95896f,34.926636f,34.894512f,34.86258f,34.830845f,34.799297f,34.76794f,34.73677f,34.705788f,34.674988f,34.644367f,34.61393f,34.583668f,34.553585f,34.523674f,34.49394f,34.46437f,34.434975f,34.405746f,34.376682f,34.347786f,34.31905f,34.290474f,34.26206f,34.233803f,34.205704f,34.177757f,34.149967f,34.122326f,34.094837f,34.067497f,34.040306f,34.013264f,33.986362f,33.959606f,33.932995f,33.90652f,33.880188f,33.853996f,33.827938f,33.802017f,33.776234f,33.75058f,33.72506f,33.699673f,33.67441f,33.649284f,33.624283f,33.599407f,33.574654f,33.55003f,33.525528f,33.50115f,33.476887f,33.452747f,33.428726f,33.404823f,33.38104f,33.35737f,33.333817f,33.310375f,33.28705f,33.263832f,33.240726f,33.21773f,33.194847f,33.172073f,33.149403f,33.12684f,33.10438f,33.08203f,33.059784f,33.037636f,33.015594f,32.993652f,32.97181f,32.95007f,32.92843f,32.906883f,32.885437f,32.864086f,32.84283f,32.821674f,32.800606f,32.779636f,32.75876f,32.737972f,32.717278f,32.69667f,32.67616f,32.65573f,32.635395f,32.615147f,32.594986f,32.57491f,32.55492f,32.53502f,32.515198f,32.495464f,32.47581f,32.45624f,32.436756f,32.41735f,32.398026f,32.37878f,32.359615f,32.34053f,32.321526f,32.302597f,32.283745f,32.26497f,32.24627f,32.227646f,32.2091f,32.190628f,32.172226f,32.1539f,32.13565f,32.11747f,32.09936f,32.081326f,32.06336f,32.045464f,32.027637f,32.009884f,31.992197f,31.974579f,31.95703f,31.939547f,31.92213f,31.904783f,31.8875f,31.870283f,31.853132f,31.836046f,31.819025f,31.802067f,31.785172f,31.768341f,31.751572f,31.734865f,31.718222f,31.70164f,31.685118f,31.668655f,31.652254f,31.635914f,31.619633f,31.603409f,31.587246f,31.57114f,31.555094f,31.539103f,31.52317f,31.507294f,31.491474f,31.475712f,31.460005f,31.444351f,31.428755f,31.413212f,31.397724f,31.38229f,31.366909f,31.351583f,31.33631f,31.321087f,31.305918f,31.290802f,31.275736f,31.260723f,31.24576f,31.230848f,31.215986f,31.201174f,31.186413f,31.171701f,31.157038f,31.142424f,31.12786f,31.113342f,31.098873f,31.084452f,31.07008f,31.055752f,31.041473f,31.02724f,31.013054f,30.998915f,30.98482f,30.97077f,30.956768f,30.942808f,30.928896f,30.915026f,30.901201f,30.88742f,30.873684f,30.85999f,30.84634f,30.832731f,30.819168f,30.805645f,30.792166f,30.778728f,30.765331f,30.751978f,30.738665f,30.725393f,30.712162f,30.69897f,30.685822f,30.672712f,30.659643f,30.646612f,30.633623f,30.62067f,30.60776f,30.594887f,30.582052f,30.569256f,30.556498f,30.54378f,30.531097f,30.518454f,30.505846f,30.493277f,30.480745f,30.46825f,30.455791f,30.443369f,30.430983f,30.418633f,30.406319f,30.39404f,30.381798f,30.369589f,30.357418f,30.34528f,30.333178f,30.32111f,30.309076f,30.297075f,30.28511f,30.27318f,30.261282f,30.249418f,30.237587f,30.22579f,30.214025f,30.202293f,30.190594f,30.178926f,30.167294f,30.155691f,30.144121f,30.132584f,30.121077f,30.109602f,30.09816f,30.086748f,30.075367f,30.064016f,30.052698f,30.041409f,30.030151f,30.018923f,30.007725f,29.996557f,29.98542f,29.974312f,29.963234f,29.952187f,29.941166f,29.930176f,29.919214f,29.908283f,29.897379f,29.886505f,29.875658f,29.86484f,29.85405f,29.843288f,29.832554f,29.821848f,29.81117f,29.80052f,29.789896f,29.7793f,29.768732f,29.75819f,29.747675f,29.737188f,29.726727f,29.716291f,29.705883f,29.695501f,29.685146f,29.674816f,29.664513f,29.654234f,29.643982f,29.633755f,29.623554f,29.613379f,29.603228f,29.593102f,29.583002f,29.572926f,29.562876f,29.55285f,29.542849f,29.532871f,29.522919f,29.51299f,29.503086f,29.493206f,29.48335f,29.473518f,29.463709f,29.453924f,29.444162f,29.434425f,29.42471f,29.415018f,29.40535f,29.395704f,29.386082f,29.376482f,29.366905f,29.357351f,29.347818f,29.338308f,29.328821f,29.319357f,29.309914f,29.300493f,29.291094f,29.281717f,29.272362f,29.26303f,29.253717f,29.244427f,29.235157f,29.22591f,29.216682f,29.207478f,29.198294f,29.189129f,29.179987f,29.170866f,29.161764f,29.152683f,29.143623f,29.134584f,29.125565f,29.116566f,29.107588f,29.098629f,29.089691f,29.080772f,29.071873f,29.062994f,29.054134f,29.045296f,29.036474f,29.027674f,29.018892f,29.010132f,29.001389f,28.992664f,28.98396f,28.975275f,28.966608f,28.95796f,28.949331f,28.940722f,28.932129f,28.923555f,28.915f,28.906466f,28.897947f,28.889448f,28.880966f,28.872503f,28.864058f,28.85563f,28.847221f,28.83883f,28.830456f,28.8221f,28.813763f,28.80544f,28.797138f,28.788853f,28.780584f,28.772333f,28.7641f,28.755882f,28.747683f,28.7395f,28.731333f,28.723185f,28.715054f,28.706938f,28.69884f,28.690758f,28.682692f,28.674644f,28.66661f,28.658596f,28.650597f,28.642612f,28.634647f,28.626696f,28.618761f,28.610842f,28.60294f,28.595053f,28.587183f,28.579329f,28.57149f,28.563665f,28.555857f,28.548065f,28.540289f,28.532526f,28.52478f,28.51705f,28.509335f,28.501635f,28.49395f,28.48628f,28.478626f,28.470987f,28.463362f,28.455753f,28.448158f,28.440578f,28.433012f,28.425463f,28.417927f,28.410404f,28.402899f,28.395407f,28.387928f,28.380465f,28.373016f,28.365582f,28.358162f,28.350756f,28.343363f,28.335985f,28.32862f,28.321272f,28.313936f,28.306614f,28.299305f,28.292011f,28.28473f,28.277464f,28.27021f,28.26297f,28.255745f,28.248531f,28.241333f,28.234148f,28.226974f,28.219816f,28.212671f,28.205538f,28.19842f,28.191313f,28.184221f,28.177141f,28.170074f,28.163021f,28.15598f,28.148952f,28.141937f,28.134935f,28.127945f,28.12097f,28.114004f,28.107054f,28.100115f,28.09319f,28.086275f,28.079374f,28.072485f,28.065609f,28.058744f,28.051893f,28.045053f,28.038227f,28.031412f,28.024609f,28.017818f,28.01104f,28.004272f,27.997519f,27.990776f,27.984045f,27.977327f,27.97062f,27.963924f,27.957241f,27.95057f,27.943909f,27.937262f,27.930624f,27.924f,27.917385f,27.910784f,27.904194f,27.897614f,27.891047f,27.88449f,27.877945f,27.87141f,27.86489f,27.858377f,27.851877f,27.845388f,27.83891f,27.832443f,27.825987f,27.819542f,27.813108f,27.806686f,27.800274f,27.793873f,27.787483f,27.781103f,27.774734f,27.768377f,27.76203f,27.755693f,27.749369f,27.743053f,27.736748f,27.730453f,27.72417f,27.717897f,27.711634f,27.705381f,27.69914f,27.69291f,27.686687f,27.680477f,27.674276f,27.668085f,27.661905f,27.655735f,27.649576f,27.643425f,27.637285f,27.631157f,27.625036f,27.618927f,27.612827f,27.606737f,27.600657f,27.594587f,27.588528f,27.582478f,27.576435f,27.570406f,27.564384f,27.558372f,27.55237f,27.54638f,27.540396f,27.534424f,27.52846f,27.522507f,27.516562f,27.510626f,27.504702f,27.498785f,27.492878f,27.48698f,27.481092f,27.475214f,27.469345f,27.463484f,27.457634f,27.451792f,27.44596f,27.440136f,27.434322f,27.428516f,27.42272f,27.416933f,27.411156f,27.405386f,27.399626f,27.393875f,27.388132f,27.382399f,27.376675f,27.370958f,27.365252f,27.359554f,27.353865f,27.348185f,27.342512f,27.33685f,27.331194f,27.325548f,27.319912f,27.314283f,27.308662f,27.303051f,27.297447f,27.291853f,27.286268f,27.28069f,27.27512f,27.26956f,27.264008f,27.258463f,27.252928f,27.2474f,27.24188f,27.23637f,27.230867f,27.225374f,27.219887f,27.214409f,27.208939f,27.203478f,27.198025f,27.19258f,27.187141f,27.181711f,27.17629f,27.170877f,27.165472f,27.160074f,27.154684f,27.149303f,27.14393f,27.138563f,27.133205f,27.127855f,27.122513f,27.117178f,27.111853f,27.106533f,27.101221f,27.095919f,27.090622f,27.085335f,27.080053f,27.07478f,27.069515f,27.064257f,27.059008f,27.053764f,27.048529f,27.043303f,27.038082f,27.03287f,27.027664f,27.022467f,27.017275f,27.012093f,27.006916f,27.001749f,26.996588f,26.991434f,26.986288f,26.981148f,26.976017f,26.970892f,26.965775f,26.960665f,26.95556f,26.950466f,26.945377f,26.940296f,26.93522f,26.930153f,26.925093f,26.92004f,26.914993f,26.909954f,26.904922f,26.899897f,26.894878f,26.889868f,26.884863f,26.879866f,26.874876f,26.869892f,26.864914f,26.859945f,26.854982f,26.850025f,26.845076f,26.840134f,26.835197f,26.830267f,26.825346f,26.820429f,26.81552f,26.810617f,26.805721f,26.800833f,26.79595f,26.791075f,26.786205f,26.781342f,26.776485f,26.771635f,26.766792f,26.761955f,26.757126f,26.752302f,26.747484f,26.742674f,26.73787f,26.733072f,26.728281f,26.723495f,26.718716f,26.713943f,26.709177f,26.704418f,26.699665f,26.694918f,26.690176f,26.685442f,26.680714f,26.675991f,26.671274f,26.666565f,26.661861f,26.657164f,26.652473f,26.647787f,26.643108f,26.638435f,26.63377f,26.629108f,26.624454f,26.619804f,26.615162f,26.610527f,26.605896f,26.60127f,26.596653f,26.592041f,26.587433f,26.582832f,26.578238f,26.573648f,26.569067f,26.56449f,26.559917f,26.555353f,26.550793f,26.54624f,26.541693f,26.53715f,26.532614f,26.528084f,26.523558f,26.51904f,26.514526f,26.51002f,26.505516f,26.50102f,26.49653f,26.492046f,26.487566f,26.483093f,26.478624f,26.474163f,26.469706f,26.465256f,26.46081f,26.45637f,26.451937f,26.447508f,26.443085f,26.438665f,26.434254f,26.429848f,26.425446f,26.42105f,26.41666f,26.412275f,26.407896f,26.40352f,26.399153f,26.394789f,26.39043f,26.386078f,26.381731f,26.37739f,26.373053f,26.368723f,26.364397f,26.360075f,26.35576f,26.35145f,26.347145f,26.342846f,26.338552f,26.334263f,26.329979f,26.3257f,26.321426f,26.317158f,26.312895f,26.308638f,26.304384f,26.300137f,26.295895f,26.291656f,26.287424f,26.283197f,26.278975f,26.274757f,26.270546f,26.266338f,26.262136f,26.25794f,26.253748f,26.249561f,26.245378f,26.241201f,26.23703f,26.232862f,26.2287f,26.224543f,26.22039f,26.216244f,26.212101f,26.207964f,26.20383f,26.199703f,26.19558f,26.191462f,26.187347f,26.183239f,26.179136f,26.175037f,26.170942f,26.166853f,26.16277f,26.15869f,26.154615f,26.150545f,26.146479f,26.142418f,26.138363f,26.134312f,26.130264f,26.126223f,26.122185f,26.118153f,26.114124f,26.110102f,26.106083f,26.102068f,26.098059f,26.094053f,26.090054f,26.086058f,26.082067f,26.078081f,26.074099f,26.070122f,26.066149f,26.06218f,26.058216f,26.054256f,26.050303f,26.046352f,26.042406f,26.038465f,26.034529f,26.030596f,26.026669f,26.022743f,26.018826f,26.01491f,26.011f,26.007093f,26.003193f,25.999296f,25.995403f,25.991514f,25.98763f,25.983751f,25.979876f,25.976006f,25.972137f,25.968275f,25.964418f,25.960564f,25.956715f,25.95287f,25.949028f,25.945192f,25.941359f,25.93753f,25.933706f,25.929888f,25.926071f,25.92226f,25.918453f,25.91465f,25.910852f,25.907057f,25.903267f,25.89948f};
class mydspSIG5 {
	
  private:
	
	int fmydspSIG5Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG5() {
		return 0;
	}
	int getNumOutputsmydspSIG5() {
		return 1;
	}
	
	void instanceInitmydspSIG5(int sample_rate) {
		fmydspSIG5Wave0_idx = 0;
	}
	
	void fillmydspSIG5(int count, float* table) {
		for (int i6 = 0; i6 < count; i6 = i6 + 1) {
			table[i6] = fmydspSIG5Wave0[fmydspSIG5Wave0_idx];
			fmydspSIG5Wave0_idx = (1 + fmydspSIG5Wave0_idx) % 2001;
		}
	}

};

static mydspSIG5* newmydspSIG5() { return (mydspSIG5*)new mydspSIG5(); }
static void deletemydspSIG5(mydspSIG5* dsp) { delete dsp; }

const static float fmydspSIG6Wave0[2001] = {249.98708f,249.98685f,249.98663f,249.9864f,249.98618f,249.98595f,249.9857f,249.98546f,249.98521f,249.98497f,249.98471f,249.98445f,249.98419f,249.98393f,249.98366f,249.98338f,249.98311f,249.98282f,249.98253f,249.98224f,249.98193f,249.98163f,249.98132f,249.981f,249.98068f,249.98036f,249.98003f,249.97969f,249.97935f,249.979f,249.97865f,249.97829f,249.97792f,249.97755f,249.97717f,249.97679f,249.9764f,249.976f,249.9756f,249.97519f,249.97476f,249.97435f,249.9739f,249.97348f,249.97302f,249.97256f,249.9721f,249.97163f,249.97116f,249.97067f,249.97018f,249.96968f,249.96918f,249.96866f,249.96812f,249.96759f,249.96704f,249.96649f,249.96593f,249.96535f,249.96477f,249.96417f,249.96358f,249.96297f,249.96234f,249.96172f,249.96106f,249.9604f,249.95975f,249.95906f,249.95837f,249.95769f,249.95697f,249.95625f,249.9555f,249.95476f,249.95401f,249.95323f,249.95244f,249.95164f,249.95084f,249.95001f,249.94917f,249.94832f,249.94745f,249.94656f,249.94566f,249.94476f,249.94383f,249.94289f,249.94193f,249.94096f,249.93997f,249.93896f,249.93794f,249.93689f,249.93584f,249.93475f,249.93367f,249.93256f,249.93143f,249.93028f,249.92911f,249.92792f,249.92671f,249.92549f,249.92424f,249.92297f,249.92168f,249.92036f,249.91904f,249.91768f,249.9163f,249.9149f,249.91348f,249.91203f,249.91055f,249.90906f,249.90755f,249.90599f,249.90442f,249.90283f,249.9012f,249.89955f,249.89787f,249.89616f,249.89444f,249.89267f,249.89088f,249.88905f,249.8872f,249.88533f,249.8834f,249.88147f,249.87949f,249.87749f,249.87544f,249.87337f,249.87126f,249.86911f,249.86693f,249.86472f,249.86246f,249.86017f,249.85785f,249.85548f,249.85307f,249.85063f,249.84814f,249.84563f,249.84305f,249.84045f,249.8378f,249.8351f,249.83237f,249.82957f,249.82675f,249.82388f,249.82095f,249.818f,249.81497f,249.8119f,249.80878f,249.80562f,249.8024f,249.79912f,249.79579f,249.79242f,249.78899f,249.78549f,249.78195f,249.77834f,249.77467f,249.77095f,249.76717f,249.76332f,249.75941f,249.75545f,249.75142f,249.74731f,249.74315f,249.73892f,249.73462f,249.73026f,249.72581f,249.7213f,249.71672f,249.71205f,249.70732f,249.70251f,249.69763f,249.69267f,249.68762f,249.6825f,249.67729f,249.672f,249.66663f,249.66116f,249.65562f,249.65f,249.64426f,249.63844f,249.63254f,249.62654f,249.62044f,249.61424f,249.60796f,249.60156f,249.59506f,249.58847f,249.58177f,249.57497f,249.56805f,249.56104f,249.55391f,249.54666f,249.5393f,249.53183f,249.52425f,249.51654f,249.5087f,249.50075f,249.49268f,249.48448f,249.47615f,249.4677f,249.4591f,249.45038f,249.44153f,249.43253f,249.4234f,249.41412f,249.40471f,249.39514f,249.38544f,249.37556f,249.36555f,249.3554f,249.34508f,249.3346f,249.32396f,249.31317f,249.3022f,249.29108f,249.27977f,249.26831f,249.25667f,249.24484f,249.23285f,249.22067f,249.2083f,249.19576f,249.18301f,249.17009f,249.15697f,249.14366f,249.13014f,249.11642f,249.10251f,249.08838f,249.07405f,249.0595f,249.04474f,249.02975f,249.01456f,248.99913f,248.98349f,248.9676f,248.9515f,248.93517f,248.91858f,248.90175f,248.88469f,248.86737f,248.84981f,248.832f,248.81392f,248.7956f,248.777f,248.75813f,248.73901f,248.7196f,248.69994f,248.67998f,248.65974f,248.63922f,248.61841f,248.5973f,248.57591f,248.55421f,248.53221f,248.5099f,248.48729f,248.46436f,248.44112f,248.41756f,248.39366f,248.36945f,248.3449f,248.32002f,248.2948f,248.26923f,248.24332f,248.21707f,248.19046f,248.16348f,248.13615f,248.10846f,248.0804f,248.05196f,248.02315f,247.99396f,247.96439f,247.93442f,247.90407f,247.8733f,247.84216f,247.81061f,247.77864f,247.74626f,247.71347f,247.68027f,247.64662f,247.61256f,247.57806f,247.54314f,247.50777f,247.47195f,247.4357f,247.39899f,247.36182f,247.32417f,247.28609f,247.24751f,247.20848f,247.16898f,247.12898f,247.0885f,247.04753f,247.00607f,246.96411f,246.92166f,246.8787f,246.83522f,246.79123f,246.74672f,246.7017f,246.65614f,246.61006f,246.56345f,246.5163f,246.46861f,246.42036f,246.37158f,246.32224f,246.27232f,246.22186f,246.17084f,246.11925f,246.06708f,246.01433f,245.961f,245.90707f,245.85257f,245.79747f,245.74178f,245.68549f,245.62859f,245.57109f,245.51297f,245.45424f,245.39488f,245.33492f,245.2743f,245.21309f,245.15121f,245.08871f,245.02557f,244.96179f,244.89737f,244.83229f,244.76656f,244.70015f,244.6331f,244.56538f,244.497f,244.42795f,244.35822f,244.28781f,244.21672f,244.14496f,244.07251f,243.99936f,243.92552f,243.851f,243.77577f,243.69984f,243.6232f,243.54587f,243.46782f,243.38905f,243.30959f,243.2294f,243.14848f,243.06685f,242.9845f,242.90141f,242.8176f,242.73305f,242.64778f,242.56175f,242.475f,242.38751f,242.29927f,242.21028f,242.12056f,242.03008f,241.93884f,241.84686f,241.75412f,241.66063f,241.56638f,241.47136f,241.37558f,241.27904f,241.18173f,241.08366f,240.98482f,240.88521f,240.78482f,240.68367f,240.58174f,240.47903f,240.37555f,240.27129f,240.16624f,240.06042f,239.95383f,239.84645f,239.73828f,239.62933f,239.51959f,239.40907f,239.29776f,239.18565f,239.07277f,238.95909f,238.84462f,238.72937f,238.61333f,238.49648f,238.37885f,238.26042f,238.1412f,238.0212f,237.90039f,237.77878f,237.65639f,237.5332f,237.40921f,237.28444f,237.15886f,237.03249f,236.90533f,236.77737f,236.6486f,236.51906f,236.38872f,236.25757f,236.12564f,235.9929f,235.85938f,235.72507f,235.58995f,235.45404f,235.31734f,235.17986f,235.04156f,234.90248f,234.76262f,234.62196f,234.48051f,234.33827f,234.19525f,234.05142f,233.90683f,233.76143f,233.61525f,233.46829f,233.32054f,233.17201f,233.0227f,232.8726f,232.72173f,232.57007f,232.41762f,232.2644f,232.11041f,231.95564f,231.8001f,231.64377f,231.48668f,231.32881f,231.17017f,231.01076f,230.85057f,230.68962f,230.52791f,230.36543f,230.2022f,230.03818f,229.87341f,229.70789f,229.5416f,229.37454f,229.20674f,229.03818f,228.86887f,228.6988f,228.52798f,228.35643f,228.18411f,228.01105f,227.83723f,227.66269f,227.4874f,227.31136f,227.13458f,226.95708f,226.77882f,226.59984f,226.42012f,226.23967f,226.0585f,225.87659f,225.69395f,225.51059f,225.3265f,225.14171f,224.95618f,224.76993f,224.58296f,224.39528f,224.2069f,224.01778f,223.82797f,223.63744f,223.4462f,223.25427f,223.06161f,222.86827f,222.67422f,222.47948f,222.28404f,222.0879f,221.89107f,221.69354f,221.49533f,221.29643f,221.09685f,220.89658f,220.69562f,220.49397f,220.29166f,220.08865f,219.88498f,219.68063f,219.47562f,219.26993f,219.06357f,218.85654f,218.64885f,218.44049f,218.23148f,218.0218f,217.81146f,217.60048f,217.38882f,217.17653f,216.96358f,216.74997f,216.53572f,216.32083f,216.1053f,215.88911f,215.6723f,215.45483f,215.23674f,215.01802f,214.79865f,214.57866f,214.35803f,214.13678f,213.9149f,213.6924f,213.46928f,213.24553f,213.02116f,212.79619f,212.5706f,212.34439f,212.11757f,211.89014f,211.6621f,211.43346f,211.20421f,210.97435f,210.74391f,210.51285f,210.2812f,210.04897f,209.81613f,209.58269f,209.34868f,209.11406f,208.87888f,208.6431f,208.40672f,208.16978f,207.93227f,207.69415f,207.45547f,207.21623f,206.9764f,206.73601f,206.49504f,206.25351f,206.01141f,205.76874f,205.52551f,205.28174f,205.03738f,204.79248f,204.54703f,204.30103f,204.05446f,203.80734f,203.55968f,203.31148f,203.06271f,202.81342f,202.56358f,202.3132f,202.06229f,201.81084f,201.55884f,201.30632f,201.05327f,200.79968f,200.54558f,200.29094f,200.03577f,199.78008f,199.52388f,199.26715f,199.0099f,198.75214f,198.49385f,198.23506f,197.97577f,197.71596f,197.45563f,197.19481f,196.93347f,196.67163f,196.4093f,196.14645f,195.88312f,195.6193f,195.35497f,195.09013f,194.82483f,194.55902f,194.29272f,194.02596f,193.75868f,193.49094f,193.2227f,192.954f,192.68481f,192.41515f,192.145f,191.87439f,191.6033f,191.33174f,191.05972f,190.78723f,190.51427f,190.24084f,189.96696f,189.69263f,189.41782f,189.14256f,188.86684f,188.59067f,188.31406f,188.03699f,187.75946f,187.48149f,187.20308f,186.92422f,186.64493f,186.3652f,186.08502f,185.80441f,185.52336f,185.24188f,184.95998f,184.67764f,184.39487f,184.11168f,183.82806f,183.54402f,183.25957f,182.9747f,182.6894f,182.4037f,182.11757f,181.83104f,181.54411f,181.25676f,180.96901f,180.68086f,180.3923f,180.10335f,179.81401f,179.52426f,179.23413f,178.9436f,178.6527f,178.36139f,178.0697f,177.77765f,177.4852f,177.19238f,176.8992f,176.60564f,176.31169f,176.0174f,175.72273f,175.4277f,175.13232f,174.8366f,174.5405f,174.24405f,173.94727f,173.65013f,173.35265f,173.05482f,172.75668f,172.45819f,172.15938f,171.86024f,171.56078f,171.261f,170.9609f,170.66049f,170.35977f,170.05875f,169.75743f,169.45581f,169.1539f,168.8517f,168.54922f,168.24646f,167.94342f,167.64012f,167.33655f,167.03271f,166.72862f,166.42429f,166.11969f,165.81487f,165.50981f,165.20453f,164.89902f,164.59329f,164.28737f,163.98123f,163.6749f,163.36838f,163.06168f,162.75479f,162.44774f,162.14053f,161.83318f,161.52568f,161.21803f,160.91028f,160.60239f,160.2944f,159.98631f,159.67813f,159.36987f,159.06155f,158.75317f,158.44473f,158.13628f,157.82779f,157.51929f,157.21078f,156.90228f,156.59383f,156.2854f,155.97704f,155.66873f,155.3605f,155.05238f,154.74437f,154.43648f,154.12874f,153.82117f,153.51376f,153.20656f,152.89955f,152.59279f,152.28629f,151.98004f,151.67409f,151.36844f,151.06313f,150.75816f,150.45357f,150.14938f,149.8456f,149.54227f,149.23938f,148.937f,148.63512f,148.33379f,148.033f,147.7328f,147.43323f,147.13428f,146.836f,146.53842f,146.24155f,145.94543f,145.65009f,145.35555f,145.06183f,144.76897f,144.477f,144.18596f,143.89586f,143.60674f,143.3186f,143.03152f,142.7455f,142.46056f,142.17674f,141.89409f,141.61261f,141.33234f,141.0533f,140.77553f,140.49904f,140.22389f,139.95007f,139.67764f,139.4066f,139.137f,138.86884f,138.60216f,138.33699f,138.07333f,137.81123f,137.55069f,137.29175f,137.03442f,136.77872f,136.52467f,136.2723f,136.0216f,135.77261f,135.52534f,135.27982f,135.03603f,134.794f,134.55374f,134.31528f,134.0786f,133.84373f,133.61067f,133.37943f,133.15001f,132.92242f,132.69667f,132.47275f,132.25067f,132.03044f,131.81206f,131.5955f,131.3808f,131.16794f,130.95691f,130.74773f,130.54036f,130.33482f,130.1311f,129.92921f,129.72911f,129.53082f,129.33432f,129.1396f,128.94667f,128.7555f,128.56607f,128.37839f,128.19243f,128.00821f,127.82569f,127.64487f,127.46573f,127.28827f,127.11246f,126.93829f,126.765755f,126.59484f,126.42552f,126.25779f,126.09163f,125.92703f,125.76398f,125.60245f,125.44243f,125.283905f,125.12686f,124.97129f,124.81717f,124.66448f,124.513214f,124.36335f,124.214874f,124.06777f,123.92203f,123.777626f,123.634544f,123.49278f,123.35231f,123.21313f,123.0752f,122.93854f,122.8031f,122.668884f,122.53588f,122.40407f,122.27343f,122.14395f,122.01563f,121.888435f,121.76237f,121.637405f,121.513535f,121.39075f,121.26902f,121.14835f,121.028725f,120.91012f,120.79253f,120.67594f,120.56034f,120.44572f,120.332054f,120.21935f,120.10758f,119.99674f,119.88682f,119.7778f,119.66968f,119.56244f,119.45607f,119.35056f,119.2459f,119.14208f,119.03909f,118.93692f,118.835556f,118.73499f,118.635216f,118.53622f,118.43799f,118.340515f,118.2438f,118.14782f,118.052574f,117.958046f,117.864235f,117.77113f,117.67872f,117.587f,117.49596f,117.405594f,117.31589f,117.22684f,117.138435f,117.05067f,116.96354f,116.87704f,116.791145f,116.705864f,116.62119f,116.53711f,116.45362f,116.37071f,116.288376f,116.20661f,116.125404f,116.04476f,115.96466f,115.8851f,115.806076f,115.727585f,115.64962f,115.572174f,115.49524f,115.41881f,115.34288f,115.26744f,115.1925f,115.11804f,115.04406f,114.97055f,114.897514f,114.824936f,114.75282f,114.68115f,114.60994f,114.53917f,114.468834f,114.39893f,114.32946f,114.260414f,114.19179f,114.12357f,114.05578f,113.98838f,113.921394f,113.854805f,113.788605f,113.72279f,113.65737f,113.59234f,113.52767f,113.46339f,113.39947f,113.33592f,113.272736f,113.20991f,113.14744f,113.08531f,113.023544f,112.96212f,112.90103f,112.84029f,112.77988f,112.719795f,112.66004f,112.60062f,112.54152f,112.482735f,112.42426f,112.366104f,112.30826f,112.25072f,112.19348f,112.13655f,112.07991f,112.023575f,111.96752f,111.911766f,111.85629f,111.80111f,111.7462f,111.691574f,111.63723f,111.58315f,111.52935f,111.475815f,111.42255f,111.369545f,111.3168f,111.26432f,111.2121f,111.16013f,111.10841f,111.056946f,111.00573f,110.95476f,110.90403f,110.853546f,110.8033f,110.753296f,110.70352f,110.653984f,110.604675f,110.555595f,110.506744f,110.45812f,110.40972f,110.36154f,110.31358f,110.26584f,110.218315f,110.171005f,110.12391f,110.07703f,110.03035f,109.98388f,109.937614f,109.891556f,109.8457f,109.80004f,109.754585f,109.70933f,109.66427f,109.6194f,109.57472f,109.53024f,109.48595f,109.44184f,109.39792f,109.35419f,109.31064f,109.26727f,109.22409f,109.18108f,109.13825f,109.095604f,109.05313f,109.010826f,108.968704f,108.92674f,108.88496f,108.843346f,108.801895f,108.76061f,108.7195f,108.67854f,108.63776f,108.59712f,108.556656f,108.51635f,108.4762f,108.4362f,108.39637f,108.35668f,108.317154f,108.27778f,108.23855f,108.19948f,108.16055f,108.12177f,108.08314f,108.044655f,108.00632f,107.96812f,107.93006f,107.89215f,107.854385f,107.81675f,107.77926f,107.741905f,107.70469f,107.66761f,107.63067f,107.59386f,107.557175f,107.52063f,107.484215f,107.44794f,107.41178f,107.375755f,107.33986f,107.30409f,107.26845f,107.23293f,107.19754f,107.16227f,107.12713f,107.0921f,107.0572f,107.02242f,106.987755f,106.95322f,106.91879f,106.88448f,106.850296f,106.81622f,106.782265f,106.74842f,106.71468f,106.68107f,106.64757f,106.614174f,106.58089f,106.547714f,106.514656f,106.481705f,106.44885f,106.416115f,106.383484f,106.35096f,106.31854f,106.28623f,106.25402f,106.22192f,106.18991f,106.158005f,106.126205f,106.09451f,106.06291f,106.03142f,106.000015f,105.96872f,105.937515f,105.90641f,105.875404f,105.84449f,105.813675f,105.78296f,105.752335f,105.7218f,105.69137f,105.66102f,105.63077f,105.60061f,105.57054f,105.540565f,105.51068f,105.48088f,105.45117f,105.421555f,105.39202f,105.36258f,105.33323f,105.303955f,105.27477f,105.24568f,105.21667f,105.187744f,105.158905f,105.13014f,105.10147f,105.072876f,105.04437f,105.015945f,104.9876f,104.959335f,104.93115f,104.90305f,104.87503f,104.847084f,104.81922f,104.791435f,104.76373f,104.7361f,104.70855f,104.681076f,104.65368f,104.62636f,104.599106f,104.57194f,104.54484f,104.51782f,104.49087f,104.464f,104.437195f,104.41047f,104.38381f,104.35723f,104.33072f,104.30428f,104.277916f,104.25162f,104.22539f,104.199234f,104.17314f,104.147125f,104.12118f,104.0953f,104.06948f,104.04374f,104.01806f,103.992455f,103.96691f,103.94144f,103.91602f,103.89068f,103.8654f,103.84019f,103.81504f,103.789955f,103.76494f,103.73998f,103.71509f,103.69026f,103.6655f,103.64079f,103.61615f,103.59157f,103.567055f,103.5426f,103.5182f,103.49387f,103.4696f,103.44539f,103.421234f,103.39714f,103.37311f,103.34914f,103.325226f,103.30137f,103.27757f,103.25383f,103.23015f,103.20652f,103.18295f,103.15945f,103.135994f,103.112595f,103.08926f,103.06597f,103.04274f,103.01957f,102.99645f,102.97339f,102.95038f,102.92742f,102.904526f,102.881676f,102.85889f,102.83614f,102.81346f,102.790825f,102.76825f,102.74572f,102.72324f,102.70082f,102.67845f,102.65613f,102.63386f,102.61164f,102.58948f,102.56736f,102.545296f,102.52328f,102.50131f,102.4794f,102.457535f,102.435715f,102.41395f,102.39223f,102.37056f,102.34894f,102.32736f,102.30584f,102.28436f,102.26293f,102.24155f,102.220215f,102.19893f,102.17769f,102.156494f,102.135345f,102.11424f,102.093185f,102.072174f,102.05121f,102.0303f,102.00942f,101.988594f,101.967804f,101.94707f,101.92638f,101.90572f,101.88512f,101.864555f,101.84404f,101.82356f,101.80313f,101.782745f,101.7624f,101.742096f,101.72184f,101.70162f,101.68145f,101.661316f,101.64123f,101.62118f,101.60117f,101.58121f,101.56129f,101.541405f,101.52156f,101.50176f,101.482f,101.46229f,101.442604f,101.422966f,101.40337f,101.38381f,101.364296f,101.34482f,101.32538f,101.30598f,101.28661f,101.26729f,101.24801f,101.22877f,101.20956f,101.19039f,101.17126f,101.15217f,101.13312f,101.1141f,101.09512f,101.07618f,101.05728f,101.038414f,101.019585f,101.00079f,100.98203f,100.96332f,100.94463f,100.92599f,100.90738f,100.88881f,100.87027f,100.85177f,100.833305f,100.81487f,100.79648f,100.778114f,100.759796f,100.7415f,100.72325f,100.70503f,100.686844f,100.66869f,100.65057f,100.63249f,100.61445f,100.596436f,100.57845f,100.56051f,100.542595f,100.52472f,100.50687f,100.48906f,100.471275f,100.45353f,100.43582f,100.41814f,100.40049f,100.38287f,100.365295f,100.34774f,100.33022f,100.31274f,100.29528f,100.27786f,100.26047f,100.24311f,100.225784f,100.20849f,100.19122f,100.17399f,100.156784f,100.13962f,100.122475f,100.10537f,100.08829f,100.071236f,100.05422f,100.03723f,100.02027f,100.00335f,99.98645f,99.96958f,99.95274f,99.935936f,99.91916f,99.90241f,99.88569f,99.868996f,99.85234f,99.83571f,99.81911f,99.80253f,99.78599f,99.76947f,99.75298f,99.73652f,99.72009f,99.70369f,99.68732f,99.67097f,99.65465f,99.63836f,99.62209f,99.605865f,99.58965f,99.57348f,99.55733f,99.5412f,99.5251f,99.50903f,99.49299f,99.476974f,99.46098f,99.44502f,99.429085f,99.41318f,99.3973f,99.38144f,99.365616f,99.34981f,99.33404f,99.31828f,99.30256f,99.286865f,99.27119f,99.25555f,99.23992f,99.22433f,99.20876f,99.193214f,99.1777f,99.1622f,99.146736f,99.131294f,99.115875f,99.10049f,99.08511f,99.06977f,99.05446f,99.03916f,99.023895f,99.00865f,98.99343f,98.97823f,98.963066f,98.94792f,98.93279f,98.917694f,98.902626f,98.88757f,98.87254f,98.857544f,98.84257f,98.82761f,98.812675f,98.79777f,98.78288f,98.76802f,98.75319f,98.73837f,98.72358f,98.70881f,98.69406f,98.679344f,98.66464f,98.64996f,98.63531f,98.62068f,98.60607f,98.591484f,98.57692f,98.56238f,98.54786f,98.53336f,98.51888f,98.50443f,98.490005f,98.47559f,98.461205f,98.44684f,98.432495f,98.418175f,98.40388f,98.389595f,98.37534f,98.36111f,98.34689f,98.3327f,98.31853f,98.30438f,98.29025f,98.276146f,98.262054f,98.24799f,98.23395f,98.219925f,98.20592f,98.19193f,98.17797f,98.16403f,98.150116f,98.136215f,98.12233f,98.108475f,98.094635f,98.08082f,98.06702f,98.05324f,98.03948f,98.02574f,98.012024f,97.99833f,97.98465f,97.970985f,97.95735f,97.94373f,97.93013f,97.91655f,97.90299f,97.88945f,97.87593f,97.86243f,97.848946f,97.83548f,97.82204f,97.80861f,97.795204f,97.781815f,97.76845f,97.7551f,97.74177f,97.728455f,97.715164f,97.70189f,97.68864f,97.6754f,97.66218f,97.64898f,97.635796f,97.622635f,97.60949f,97.59637f,97.58325f,97.57017f,97.55709f,97.544044f,97.531006f,97.51799f,97.50499f,97.492004f,97.47904f,97.466095f,97.45317f,97.44026f,97.42737f,97.41449f,97.401634f,97.388794f,97.37598f,97.36317f,97.35038f,97.33761f,97.32486f,97.312126f,97.29941f,97.286705f,97.27402f,97.26135f,97.2487f,97.23607f,97.22346f,97.21085f,97.19827f,97.18571f,97.17316f,97.16063f,97.14811f,97.13561f,97.12313f,97.110664f,97.09822f,97.085785f,97.07337f,97.06097f,97.048584f,97.03622f,97.023865f,97.011536f,96.999214f,96.986916f,96.974625f,96.96236f,96.9501f,96.93787f,96.925644f,96.91344f,96.901245f,96.88907f,96.876915f,96.86477f,96.85264f,96.84053f,96.82843f,96.81635f,96.80429f,96.79224f,96.780205f,96.76818f,96.75618f,96.744194f,96.73222f,96.72026f,96.70832f,96.696396f,96.68448f,96.672585f,96.660706f,96.648834f,96.636986f,96.625145f,96.61333f,96.60152f,96.58972f,96.57795f,96.566185f,96.554436f,96.5427f,96.53098f,96.51928f,96.50759f,96.49592f,96.48425f,96.47261f,96.460976f,96.44936f,96.43776f,96.42617f,96.4146f,96.40304f,96.391495f,96.37996f,96.36845f,96.35694f,96.34545f,96.33398f,96.32252f,96.31107f,96.29964f,96.288216f,96.27681f,96.26542f,96.25404f,96.24268f,96.23133f,96.21999f,96.20867f,96.197365f,96.18607f,96.17479f,96.16352f,96.15227f,96.14103f,96.1298f,96.11858f,96.10738f,96.0962f,96.08502f,96.07386f,96.06271f,96.05158f,96.04046f,96.02936f,96.01826f,96.00718f,95.99611f,95.985054f,95.974014f,95.96298f,95.95197f,95.94096f,95.92998f,95.919f,95.908035f,95.89709f,95.88615f,95.87522f,95.8643f,95.85341f,95.84252f,95.83164f,95.82078f,95.80993f,95.799095f,95.78827f,95.77746f,95.766655f,95.75587f,95.745094f,95.73434f,95.72358f,95.712845f,95.70212f,95.69141f,95.68071f,95.67002f,95.65934f,95.648674f,95.63802f,95.62739f,95.61676f,95.60614f,95.595535f,95.584946f,95.57436f,95.5638f,95.553246f,95.5427f,95.532166f,95.521645f,95.51114f,95.50064f,95.49016f,95.47968f,95.46922f,95.45877f,95.44833f,95.43791f,95.4275f,95.41709f,95.4067f,95.396324f,95.385956f,95.375595f,95.36525f,95.35492f,95.3446f,95.33428f,95.32398f,95.3137f,95.30342f,95.29315f,95.2829f,95.27265f,95.26242f,95.252205f,95.24199f,95.2318f,95.2216f,95.211426f,95.20126f,95.19111f,95.18097f,95.17084f,95.16071f,95.150604f,95.1405f,95.13042f,95.12034f,95.110275f,95.10022f,95.09017f,95.08014f,95.070114f,95.060104f,95.0501f,95.04011f,95.03013f,95.02016f,95.0102f,95.00025f,94.99032f,94.980385f,94.970474f,94.96056f,94.95067f,94.94078f,94.93091f,94.92104f,94.911194f,94.90135f,94.89152f,94.88169f,94.87188f,94.862076f,94.85229f,94.84251f,94.83273f,94.822975f,94.813225f,94.80348f,94.793755f,94.784035f,94.77432f,94.76462f,94.75493f,94.745255f,94.73558f,94.72592f,94.71627f,94.706635f,94.69701f,94.687386f,94.67777f,94.668175f,94.658585f,94.649f,94.639435f,94.629875f,94.62032f,94.61078f,94.60125f,94.59173f,94.582214f,94.572716f,94.56322f,94.55374f,94.544266f,94.5348f,94.525345f,94.5159f,94.50647f,94.49704f,94.487625f,94.47822f,94.46883f,94.459435f,94.45006f,94.44069f,94.431335f,94.42198f,94.41264f,94.40331f,94.39399f,94.38468f,94.375374f,94.36608f,94.3568f,94.34753f,94.33826f,94.329f,94.319756f,94.31052f,94.301285f,94.29207f,94.28286f,94.27366f,94.264465f,94.25528f,94.2461f,94.23694f,94.22778f,94.218636f,94.209496f,94.20037f,94.191246f,94.18214f,94.173035f,94.16394f,94.154854f,94.14578f,94.13671f,94.127655f,94.11861f,94.109566f,94.10053f,94.09151f,94.0825f,94.07349f,94.06449f,94.055504f,94.046524f,94.03755f,94.028595f,94.01964f,94.0107f,94.001755f,93.99283f,93.98391f,93.975f,93.966095f,93.95721f,93.94832f,93.939445f,93.93057f,93.921715f,93.912865f,93.90402f,93.89519f,93.88636f,93.87754f,93.86874f,93.85993f,93.85114f,93.842354f,93.83358f,93.824814f,93.816055f,93.807304f,93.79856f,93.789825f,93.7811f,93.77238f,93.763664f};
class mydspSIG6 {
	
  private:
	
	int fmydspSIG6Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG6() {
		return 0;
	}
	int getNumOutputsmydspSIG6() {
		return 1;
	}
	
	void instanceInitmydspSIG6(int sample_rate) {
		fmydspSIG6Wave0_idx = 0;
	}
	
	void fillmydspSIG6(int count, float* table) {
		for (int i7 = 0; i7 < count; i7 = i7 + 1) {
			table[i7] = fmydspSIG6Wave0[fmydspSIG6Wave0_idx];
			fmydspSIG6Wave0_idx = (1 + fmydspSIG6Wave0_idx) % 2001;
		}
	}

};

static mydspSIG6* newmydspSIG6() { return (mydspSIG6*)new mydspSIG6(); }
static void deletemydspSIG6(mydspSIG6* dsp) { delete dsp; }

const static float fmydspSIG7Wave0[2001] = {223.48497f,223.3819f,223.27861f,223.1751f,223.07137f,222.96742f,222.86325f,222.75887f,222.65427f,222.54945f,222.44441f,222.33916f,222.23369f,222.128f,222.0221f,221.91597f,221.80963f,221.70308f,221.59631f,221.48933f,221.38213f,221.2747f,221.16708f,221.05923f,220.95117f,220.8429f,220.7344f,220.6257f,220.51678f,220.40765f,220.29831f,220.18875f,220.07898f,219.969f,219.8588f,219.7484f,219.63777f,219.52693f,219.4159f,219.30464f,219.19316f,219.08148f,218.96959f,218.8575f,218.74518f,218.63266f,218.51993f,218.40698f,218.29384f,218.18047f,218.06691f,217.95312f,217.83913f,217.72493f,217.61053f,217.49591f,217.38109f,217.26607f,217.15083f,217.03539f,216.91972f,216.80386f,216.6878f,216.57153f,216.45505f,216.33836f,216.22148f,216.10439f,215.98708f,215.86957f,215.75186f,215.63394f,215.51582f,215.39749f,215.27896f,215.16023f,215.04129f,214.92215f,214.80281f,214.68326f,214.5635f,214.44356f,214.3234f,214.20303f,214.08247f,213.96172f,213.84074f,213.71959f,213.59822f,213.47665f,213.35489f,213.23291f,213.11075f,212.98837f,212.86581f,212.74304f,212.62007f,212.4969f,212.37354f,212.24997f,212.1262f,212.00224f,211.87808f,211.75372f,211.62918f,211.50443f,211.37947f,211.25432f,211.12898f,211.00343f,210.8777f,210.75177f,210.62564f,210.49931f,210.37279f,210.24606f,210.11916f,209.99205f,209.86475f,209.73726f,209.60956f,209.48167f,209.35359f,209.22533f,209.09686f,208.9682f,208.83936f,208.7103f,208.58107f,208.45163f,208.322f,208.1922f,208.0622f,207.93199f,207.8016f,207.67102f,207.54025f,207.40929f,207.27814f,207.14679f,207.01526f,206.88353f,206.75162f,206.61952f,206.48723f,206.35475f,206.22208f,206.08922f,205.95618f,205.82294f,205.68951f,205.5559f,205.4221f,205.28812f,205.15393f,205.01958f,204.88503f,204.75029f,204.61537f,204.48026f,204.34497f,204.20949f,204.07382f,203.93797f,203.80194f,203.66571f,203.52931f,203.39272f,203.25594f,203.11897f,202.98183f,202.8445f,202.70699f,202.56929f,202.43141f,202.29335f,202.1551f,202.01668f,201.87807f,201.73927f,201.60031f,201.46115f,201.32181f,201.18228f,201.04259f,200.90271f,200.76263f,200.62239f,200.48196f,200.34135f,200.20058f,200.0596f,199.91846f,199.77713f,199.63562f,199.49394f,199.35208f,199.21004f,199.06781f,198.92542f,198.78284f,198.64008f,198.49713f,198.35402f,198.21072f,198.06726f,197.92361f,197.77979f,197.63579f,197.49161f,197.34726f,197.20273f,197.05801f,196.91313f,196.76808f,196.62285f,196.47743f,196.33185f,196.18608f,196.04015f,195.89404f,195.74776f,195.60129f,195.45465f,195.30785f,195.16086f,195.0137f,194.86638f,194.71887f,194.5712f,194.42334f,194.27531f,194.12712f,193.97876f,193.83022f,193.6815f,193.53261f,193.38354f,193.23433f,193.08492f,192.93535f,192.7856f,192.63568f,192.4856f,192.33534f,192.1849f,192.03432f,191.88354f,191.7326f,191.5815f,191.4302f,191.27876f,191.12714f,190.97536f,190.8234f,190.67126f,190.51897f,190.3665f,190.21387f,190.06107f,189.9081f,189.75496f,189.60165f,189.44818f,189.29454f,189.14073f,188.98676f,188.8326f,188.67828f,188.5238f,188.36917f,188.21436f,188.05937f,187.90422f,187.74892f,187.59343f,187.43779f,187.28198f,187.126f,186.96986f,186.81355f,186.65709f,186.50044f,186.34364f,186.18668f,186.02954f,185.87225f,185.71478f,185.55716f,185.39937f,185.24142f,185.0833f,184.92502f,184.76657f,184.60797f,184.4492f,184.29027f,184.13116f,183.97191f,183.81248f,183.6529f,183.49315f,183.33324f,183.17316f,183.01292f,182.85252f,182.69197f,182.53125f,182.37036f,182.20932f,182.04811f,181.88675f,181.72522f,181.56352f,181.40167f,181.23967f,181.0775f,180.91516f,180.75267f,180.59001f,180.4272f,180.26422f,180.10109f,179.9378f,179.77435f,179.61073f,179.44696f,179.28304f,179.11894f,178.9547f,178.79028f,178.62572f,178.46098f,178.29611f,178.13106f,177.96587f,177.8005f,177.63498f,177.4693f,177.30347f,177.13748f,176.97133f,176.80502f,176.63855f,176.47192f,176.30515f,176.13821f,175.97112f,175.80386f,175.63646f,175.46889f,175.30116f,175.13329f,174.96526f,174.79706f,174.62872f,174.4602f,174.29155f,174.12274f,173.95377f,173.78464f,173.61536f,173.4459f,173.27632f,173.10657f,172.93666f,172.7666f,172.59639f,172.42601f,172.2555f,172.08481f,171.91397f,171.74298f,171.57184f,171.40053f,171.22908f,171.05748f,170.88571f,170.71379f,170.54172f,170.36949f,170.19711f,170.02458f,169.8519f,169.67906f,169.50607f,169.33292f,169.15962f,168.98618f,168.81256f,168.63881f,168.46489f,168.29082f,168.11661f,167.94223f,167.76772f,167.59303f,167.4182f,167.24323f,167.06808f,166.8928f,166.71736f,166.54176f,166.36603f,166.19012f,166.01408f,165.83789f,165.66153f,165.48503f,165.30838f,165.13156f,164.9546f,164.7775f,164.60023f,164.42282f,164.24527f,164.06755f,163.88968f,163.71167f,163.5335f,163.35518f,163.17671f,162.9981f,162.81932f,162.6404f,162.46132f,162.2821f,162.10272f,161.9232f,161.74353f,161.5637f,161.38373f,161.2036f,161.02333f,160.8429f,160.66232f,160.4816f,160.30072f,160.1197f,159.93852f,159.7572f,159.57573f,159.3941f,159.21233f,159.0304f,158.84833f,158.6661f,158.48373f,158.30121f,158.11853f,157.93571f,157.75275f,157.56963f,157.38635f,157.20294f,157.01938f,156.83566f,156.6518f,156.46779f,156.28362f,156.0993f,155.91486f,155.73024f,155.54549f,155.36058f,155.17552f,154.99033f,154.80496f,154.61946f,154.43382f,154.24802f,154.06207f,153.87598f,153.68974f,153.50334f,153.3168f,153.13013f,152.94328f,152.7563f,152.56917f,152.3819f,152.19446f,152.00688f,151.81917f,151.63129f,151.44327f,151.2551f,151.06679f,150.87833f,150.68971f,150.50095f,150.31204f,150.12299f,149.93378f,149.74443f,149.55493f,149.36528f,149.17549f,148.98555f,148.79546f,148.60522f,148.41484f,148.2243f,148.03363f,147.84279f,147.65182f,147.4607f,147.26942f,147.078f,146.88644f,146.69473f,146.50287f,146.31087f,146.11871f,145.9264f,145.73395f,145.54135f,145.34862f,145.15572f,144.96268f,144.7695f,144.57616f,144.38268f,144.18904f,143.99527f,143.80135f,143.60727f,143.41306f,143.21869f,143.02419f,142.82951f,142.6347f,142.43976f,142.24466f,142.04941f,141.854f,141.65846f,141.46277f,141.26692f,141.07094f,140.8748f,140.67853f,140.4821f,140.28552f,140.0888f,139.89192f,139.69492f,139.49774f,139.30043f,139.10297f,138.90536f,138.70761f,138.5097f,138.31166f,138.11346f,137.91512f,137.71661f,137.51797f,137.3192f,137.12025f,136.92117f,136.72195f,136.52257f,136.32304f,136.12337f,135.92355f,135.72359f,135.52347f,135.32321f,135.12279f,134.92224f,134.72153f,134.52068f,134.31967f,134.11853f,133.91722f,133.71577f,133.51419f,133.31245f,133.11057f,132.90852f,132.70634f,132.504f,132.30153f,132.09889f,131.89612f,131.69319f,131.49013f,131.2869f,131.08353f,130.88002f,130.67635f,130.47253f,130.26857f,130.06447f,129.8602f,129.65579f,129.45125f,129.24654f,129.04169f,128.83669f,128.63153f,128.42624f,128.2208f,128.0152f,127.80945f,127.60356f,127.397514f,127.19132f,126.984985f,126.778496f,126.57186f,126.365074f,126.15814f,125.95106f,125.74383f,125.536446f,125.32891f,125.12123f,124.91341f,124.70543f,124.4973f,124.289024f,124.0806f,123.87202f,123.66329f,123.45442f,123.24539f,123.036224f,122.8269f,122.617424f,122.40781f,122.19803f,121.988106f,121.77804f,121.56782f,121.357445f,121.14692f,120.93625f,120.725426f,120.51446f,120.30334f,120.092064f,119.880646f,119.66907f,119.45735f,119.245476f,119.033455f,118.82128f,118.608955f,118.396484f,118.18386f,117.971085f,117.75816f,117.54508f,117.331856f,117.118484f,116.90495f,116.69128f,116.47745f,116.263466f,116.04934f,115.83506f,115.62063f,115.406044f,115.19131f,114.976425f,114.76139f,114.5462f,114.33087f,114.11538f,113.89974f,113.68395f,113.46802f,113.25192f,113.03568f,112.81929f,112.602745f,112.386055f,112.169205f,111.95221f,111.73506f,111.51777f,111.300316f,111.08272f,110.86497f,110.64707f,110.429016f,110.210815f,109.99246f,109.773964f,109.55531f,109.33651f,109.11755f,108.89845f,108.6792f,108.45979f,108.24024f,108.02054f,107.80068f,107.58068f,107.36053f,107.14023f,106.91978f,106.69917f,106.478424f,106.25753f,106.03648f,105.815285f,105.59395f,105.37246f,105.15082f,104.92903f,104.7071f,104.485016f,104.262794f,104.04042f,103.8179f,103.59524f,103.37243f,103.14947f,102.92637f,102.703125f,102.47974f,102.2562f,102.03253f,101.80871f,101.584755f,101.36065f,101.136406f,100.912025f,100.6875f,100.46284f,100.23804f,100.01309f,99.78802f,99.5628f,99.33745f,99.11196f,98.886345f,98.66059f,98.43471f,98.20869f,97.98254f,97.756256f,97.529854f,97.303314f,97.07665f,96.84987f,96.622955f,96.39592f,96.16876f,95.94149f,95.714096f,95.48658f,95.25896f,95.03121f,94.80336f,94.57539f,94.34732f,94.11914f,93.890854f,93.66247f,93.43398f,93.2054f,92.976715f,92.74794f,92.51907f,92.290115f,92.06107f,91.83195f,91.602745f,91.37347f,91.14411f,90.91468f,90.68519f,90.455635f,90.22601f,89.996346f,89.76662f,89.53684f,89.30703f,89.07717f,88.847275f,88.617355f,88.387405f,88.15744f,87.92745f,87.69746f,87.46746f,87.237465f,87.00748f,86.777504f,86.54755f,86.31762f,86.08772f,85.85787f,85.62807f,85.39832f,85.16863f,84.939026f,84.70949f,84.48005f,84.2507f,84.02146f,83.79234f,83.56335f,83.33448f,83.105774f,82.87721f,82.64883f,82.420616f,82.192604f,81.96479f,81.73719f,81.50982f,81.28269f,81.05582f,80.82921f,80.60288f,80.376854f,80.15113f,79.925735f,79.70068f,79.47598f,79.25166f,79.027725f,78.80419f,78.581085f,78.35841f,78.13621f,77.91447f,77.69322f,77.47249f,77.25229f,77.03263f,76.813545f,76.59505f,76.37716f,76.15989f,75.943275f,75.727325f,75.51206f,75.29751f,75.08369f,74.870605f,74.6583f,74.44679f,74.236084f,74.026215f,73.8172f,73.609055f,73.4018f,73.19547f,72.990074f,72.78563f,72.58216f,72.379684f,72.17822f,71.97779f,71.77842f,71.58011f,71.3829f,71.18678f,70.9918f,70.79795f,70.605255f,70.413734f,70.223404f,70.03427f,69.84636f,69.659676f,69.47423f,69.29005f,69.107124f,68.925476f,68.745125f,68.56606f,68.38831f,68.211876f,68.036766f,67.86298f,67.69054f,67.51943f,67.34967f,67.18127f,67.01421f,66.84852f,66.68418f,66.5212f,66.35959f,66.19934f,66.04045f,65.88293f,65.72676f,65.57195f,65.4185f,65.266396f,65.11565f,64.96623f,64.81816f,64.671425f,64.52602f,64.381935f,64.23916f,64.0977f,63.95754f,63.818672f,63.681087f,63.544785f,63.409748f,63.275974f,63.143448f,63.01216f,62.88211f,62.753277f,62.625656f,62.499237f,62.37401f,62.249958f,62.12708f,62.005356f,61.884777f,61.76534f,61.647026f,61.529823f,61.413723f,61.298714f,61.184788f,61.071926f,60.96012f,60.84936f,60.73963f,60.630928f,60.523235f,60.416542f,60.310833f,60.206104f,60.102337f,59.999527f,59.897655f,59.79672f,59.6967f,59.59759f,59.499382f,59.40206f,59.305614f,59.210037f,59.115314f,59.021435f,58.92839f,58.836174f,58.744766f,58.654167f,58.56436f,58.475338f,58.38709f,58.299606f,58.21288f,58.126896f,58.04165f,57.95713f,57.87333f,57.790237f,57.707844f,57.626144f,57.545124f,57.464783f,57.3851f,57.30608f,57.227707f,57.149975f,57.072876f,56.996403f,56.920547f,56.8453f,56.770653f,56.696598f,56.623135f,56.550247f,56.477936f,56.40619f,56.335f,56.26436f,56.194263f,56.12471f,56.055683f,55.987183f,55.9192f,55.851734f,55.784767f,55.718304f,55.652336f,55.586853f,55.52185f,55.457325f,55.393272f,55.32968f,55.26655f,55.203873f,55.141644f,55.079857f,55.01851f,54.957596f,54.897106f,54.83704f,54.777393f,54.71816f,54.659332f,54.600906f,54.54288f,54.485252f,54.42801f,54.371155f,54.31468f,54.25858f,54.202854f,54.147495f,54.0925f,54.037865f,53.983585f,53.92966f,53.87608f,53.822845f,53.76995f,53.717392f,53.665165f,53.61327f,53.5617f,53.510452f,53.459522f,53.40891f,53.358604f,53.308613f,53.258923f,53.209538f,53.16045f,53.11166f,53.06316f,53.01495f,52.967026f,52.919388f,52.87203f,52.824947f,52.778145f,52.73161f,52.685345f,52.639347f,52.593613f,52.54814f,52.50293f,52.45797f,52.413265f,52.368813f,52.324608f,52.280647f,52.236935f,52.19346f,52.150223f,52.107227f,52.06446f,52.02193f,51.979626f,51.93755f,51.895702f,51.854076f,51.812668f,51.771484f,51.730515f,51.68976f,51.649216f,51.608887f,51.568764f,51.52885f,51.48914f,51.449635f,51.410328f,51.371223f,51.332314f,51.293602f,51.25508f,51.21676f,51.178623f,51.140675f,51.102917f,51.065342f,51.027954f,50.990746f,50.95372f,50.916874f,50.880203f,50.843708f,50.80739f,50.771244f,50.735268f,50.699463f,50.66383f,50.62836f,50.593056f,50.557915f,50.52294f,50.488125f,50.453472f,50.418976f,50.384636f,50.350456f,50.31643f,50.282555f,50.248833f,50.215263f,50.181843f,50.14857f,50.115448f,50.08247f,50.049637f,50.01695f,49.984406f,49.952f,49.919735f,49.88761f,49.855625f,49.823776f,49.792065f,49.760487f,49.729042f,49.697735f,49.666553f,49.635506f,49.604588f,49.5738f,49.54314f,49.512604f,49.482197f,49.451912f,49.421753f,49.39172f,49.361805f,49.332012f,49.30234f,49.27279f,49.243355f,49.21404f,49.18484f,49.155758f,49.12679f,49.097935f,49.069195f,49.04057f,49.01205f,48.983646f,48.955353f,48.92717f,48.899094f,48.871124f,48.84326f,48.815506f,48.787857f,48.760315f,48.732876f,48.705536f,48.678303f,48.651173f,48.62414f,48.59721f,48.57038f,48.543648f,48.517017f,48.490482f,48.464043f,48.437702f,48.411457f,48.385307f,48.359253f,48.33329f,48.307423f,48.281647f,48.255962f,48.23037f,48.20487f,48.17946f,48.154137f,48.128906f,48.103764f,48.078705f,48.053738f,48.028854f,48.00406f,47.97935f,47.954727f,47.930187f,47.90573f,47.881355f,47.857067f,47.83286f,47.808735f,47.78469f,47.760727f,47.736843f,47.713043f,47.68932f,47.665672f,47.64211f,47.618618f,47.595207f,47.571873f,47.548615f,47.525433f,47.502327f,47.479298f,47.45634f,47.43346f,47.41065f,47.387913f,47.365253f,47.342663f,47.320145f,47.2977f,47.275322f,47.253017f,47.23078f,47.20862f,47.186523f,47.164497f,47.14254f,47.12065f,47.098827f,47.077076f,47.05539f,47.033768f,47.012215f,46.990726f,46.969303f,46.947945f,46.92665f,46.905422f,46.88426f,46.86316f,46.84212f,46.821148f,46.800236f,46.779385f,46.7586f,46.737873f,46.71721f,46.696606f,46.67606f,46.65558f,46.635155f,46.61479f,46.594486f,46.57424f,46.554054f,46.533924f,46.513855f,46.49384f,46.473885f,46.453987f,46.434143f,46.41436f,46.39463f,46.374954f,46.355335f,46.335773f,46.316265f,46.29681f,46.277412f,46.258068f,46.238777f,46.219536f,46.20035f,46.18122f,46.162144f,46.143116f,46.12414f,46.105217f,46.086346f,46.067528f,46.048756f,46.03004f,46.01137f,45.992756f,45.974186f,45.95567f,45.937202f,45.91878f,45.900414f,45.88209f,45.86382f,45.845596f,45.82742f,45.80929f,45.79121f,45.773174f,45.755188f,45.73725f,45.719357f,45.70151f,45.68371f,45.665955f,45.648247f,45.630585f,45.612965f,45.595394f,45.577866f,45.560383f,45.542942f,45.525547f,45.508198f,45.49089f,45.473625f,45.456406f,45.439228f,45.422096f,45.405006f,45.387955f,45.37095f,45.353985f,45.337063f,45.320183f,45.303345f,45.286545f,45.26979f,45.253075f,45.2364f,45.219765f,45.203175f,45.18662f,45.170105f,45.153633f,45.1372f,45.120804f,45.10445f,45.088135f,45.071857f,45.05562f,45.03942f,45.02326f,45.007137f,44.99105f,44.975006f,44.958996f,44.943024f,44.92709f,44.911194f,44.895336f,44.879513f,44.863728f,44.847977f,44.832264f,44.81659f,44.80095f,44.785343f,44.769775f,44.754242f,44.738743f,44.72328f,44.707855f,44.692463f,44.677105f,44.661785f,44.646496f,44.631245f,44.616024f,44.600838f,44.58569f,44.57057f,44.55549f,44.54044f,44.52542f,44.51044f,44.49549f,44.48057f,44.46569f,44.450836f,44.43602f,44.421234f,44.40648f,44.391758f,44.377068f,44.36241f,44.347786f,44.33319f,44.318626f,44.304096f,44.289597f,44.275127f,44.26069f,44.246284f,44.231907f,44.217564f,44.203247f,44.18896f,44.17471f,44.160484f,44.14629f,44.132126f,44.117992f,44.103886f,44.089813f,44.075768f,44.06175f,44.047764f,44.033806f,44.019875f,44.005978f,43.992104f,43.97826f,43.964447f,43.95066f,43.936905f,43.923176f,43.909473f,43.8958f,43.882156f,43.868538f,43.854946f,43.841385f,43.82785f,43.814342f,43.80086f,43.787407f,43.77398f,43.760582f,43.747208f,43.733864f,43.720543f,43.70725f,43.693985f,43.680744f,43.66753f,43.654343f,43.641182f,43.628044f,43.614933f,43.60185f,43.58879f,43.575756f,43.56275f,43.549767f,43.53681f,43.52388f,43.51097f,43.49809f,43.485233f,43.4724f,43.45959f,43.446808f,43.43405f,43.421318f,43.408607f,43.39592f,43.38326f,43.37062f,43.35801f,43.34542f,43.332855f,43.320312f,43.307793f,43.2953f,43.28283f,43.27038f,43.257954f,43.245552f,43.233177f,43.22082f,43.20849f,43.19618f,43.18389f,43.171627f,43.159386f,43.147167f,43.13497f,43.1228f,43.11065f,43.09852f,43.08641f,43.074326f,43.062263f,43.050224f,43.038204f,43.026207f,43.014233f,43.002277f,42.990345f,42.978436f,42.966545f,42.954678f,42.942833f,42.931007f,42.9192f,42.90742f,42.895657f,42.883915f,42.872196f,42.860497f,42.848816f,42.83716f,42.82552f,42.813904f,42.802307f,42.79073f,42.779175f,42.76764f,42.756123f,42.744625f,42.73315f,42.721695f,42.71026f,42.698845f,42.687447f,42.67607f,42.664715f,42.653378f,42.64206f,42.63076f,42.61948f,42.608223f,42.59698f,42.58576f,42.57456f,42.563374f,42.55221f,42.541065f,42.529938f,42.518833f,42.507744f,42.496674f,42.485622f,42.47459f,42.463573f,42.45258f,42.4416f,42.43064f,42.4197f,42.40878f,42.397877f,42.38699f,42.37612f,42.36527f,42.35444f,42.343624f,42.33283f,42.32205f,42.311287f,42.300545f,42.289818f,42.27911f,42.268417f,42.257744f,42.24709f,42.23645f,42.22583f,42.215225f,42.20464f,42.19407f,42.183514f,42.172977f,42.16246f,42.15196f,42.14147f,42.131004f,42.120552f,42.11012f,42.0997f,42.0893f,42.078915f,42.068546f,42.058193f,42.04786f,42.037537f,42.027237f,42.01695f,42.00668f,41.99642f,41.986187f,41.975964f,41.965755f,41.955566f,41.945393f,41.935234f,41.92509f,41.914963f,41.904854f,41.894756f,41.884678f,41.874615f,41.864563f,41.85453f,41.844513f,41.83451f,41.824524f,41.814552f,41.804596f,41.794655f,41.78473f,41.77482f,41.764923f,41.75504f,41.745174f,41.735325f,41.725487f,41.715668f,41.70586f,41.69607f,41.686295f,41.676533f,41.666782f,41.65705f,41.647335f,41.63763f,41.62794f,41.618267f,41.608604f,41.59896f,41.58933f,41.579712f,41.57011f,41.56052f,41.550945f,41.541386f,41.531837f,41.522305f,41.512787f,41.503284f,41.493793f,41.484318f,41.474854f,41.465405f,41.45597f,41.44655f,41.43714f,41.42775f,41.41837f,41.409f,41.399647f,41.39031f,41.38098f,41.37167f,41.36237f,41.353085f,41.34381f,41.334553f,41.325306f,41.316074f,41.306854f,41.297646f,41.288452f,41.279274f,41.270107f,41.260952f,41.251812f,41.242683f,41.233566f,41.224464f,41.215374f,41.2063f,41.19723f,41.188183f,41.179142f,41.170116f,41.161102f,41.152103f,41.143112f,41.134136f,41.125175f,41.116222f,41.107285f,41.09836f,41.089447f,41.080544f,41.071655f,41.06278f,41.053913f,41.045063f,41.03622f,41.027393f,41.018578f,41.009773f,41.00098f,40.992203f,40.983433f,40.974678f,40.965935f,40.957203f,40.948483f,40.939774f,40.931076f,40.92239f,40.91372f,40.905056f,40.896408f,40.887768f,40.879143f,40.87053f,40.861923f,40.853333f,40.844753f,40.83618f,40.827625f,40.81908f,40.810543f,40.80202f,40.79351f,40.785007f,40.77652f,40.76804f,40.759575f,40.751118f,40.742672f,40.734238f,40.72582f,40.717407f,40.709003f,40.700615f,40.692238f,40.68387f,40.675514f,40.667168f,40.658833f,40.65051f,40.642197f,40.633892f,40.625603f,40.61732f,40.60905f,40.600792f,40.592545f,40.584305f,40.576077f,40.56786f,40.559654f,40.551456f,40.543274f,40.5351f,40.526936f,40.51878f,40.510635f,40.502502f,40.49438f,40.486267f,40.478165f,40.470074f,40.46199f,40.453922f,40.445858f,40.43781f,40.429768f,40.421738f,40.413715f,40.405704f,40.397705f,40.389713f,40.381733f,40.373764f,40.365803f,40.357853f,40.34991f,40.34198f,40.33406f,40.32615f,40.318245f,40.310356f,40.302475f,40.2946f,40.28674f,40.278885f,40.27104f,40.26321f,40.255386f,40.247574f,40.23977f,40.23197f,40.22419f,40.21641f,40.208645f,40.20089f,40.193142f,40.185406f,40.177677f,40.169956f,40.162247f,40.154545f,40.146854f,40.139175f,40.1315f,40.123837f,40.116184f,40.10854f,40.100903f,40.093277f,40.08566f,40.078053f,40.070454f,40.062862f,40.055283f,40.04771f,40.04015f,40.032593f,40.02505f,40.017513f,40.009987f,40.00247f,39.99496f,39.98746f,39.97997f,39.97249f,39.965015f,39.95755f,39.950092f,39.942646f,39.935207f,39.927776f,39.920357f,39.912945f,39.90554f,39.898148f,39.89076f,39.88338f,39.876015f,39.868652f,39.8613f,39.85396f,39.846622f,39.839294f,39.831978f,39.82467f,39.817368f,39.810074f,39.80279f,39.795513f,39.788246f,39.780987f,39.77374f,39.766495f,39.759262f,39.752033f,39.744816f,39.73761f,39.730408f,39.723213f,39.71603f,39.70885f,39.701683f,39.694523f,39.68737f,39.68023f,39.67309f,39.665962f,39.658844f,39.651733f,39.644627f,39.63753f,39.630444f,39.623363f,39.616295f,39.60923f,39.602173f,39.595123f,39.588085f,39.58105f,39.574028f,39.56701f,39.56f,39.553f,39.546005f,39.53902f,39.532043f,39.525074f,39.518112f,39.511158f,39.504208f,39.49727f,39.490337f,39.483414f,39.476498f,39.46959f,39.46269f,39.45579f,39.448906f,39.442028f,39.435158f,39.428295f,39.421436f,39.41459f,39.40775f,39.400913f,39.39409f,39.387268f,39.38046f,39.373653f,39.366856f,39.360065f,39.353283f,39.346508f,39.33974f,39.33298f,39.32623f,39.319485f,39.312744f,39.306015f,39.29929f,39.292572f,39.285862f,39.27916f,39.272465f,39.265778f,39.259098f,39.252422f,39.245758f,39.239098f,39.232445f,39.2258f,39.21916f,39.21253f,39.205902f,39.199287f,39.192677f,39.186073f,39.179478f,39.172886f,39.166306f,39.15973f,39.15316f,39.1466f,39.14004f,39.133495f,39.126953f,39.12042f,39.11389f,39.10737f,39.100857f,39.09435f,39.08785f,39.081352f,39.074867f,39.068386f,39.061913f,39.055447f,39.048985f,39.04253f,39.036083f,39.029644f,39.02321f,39.016785f,39.010365f,39.00395f,38.997543f,38.991142f,38.98475f,38.97836f,38.971977f,38.965603f,38.959236f,38.952873f,38.946518f,38.94017f,38.93383f,38.927494f,38.921165f,38.91484f,38.908524f,38.902214f,38.895912f,38.889614f,38.883324f,38.877037f,38.870758f,38.864487f,38.858223f,38.851963f,38.84571f,38.839462f,38.83322f,38.82699f,38.82076f,38.814537f,38.808323f,38.802113f,38.79591f,38.78971f,38.783524f,38.777336f,38.77116f,38.764988f,38.75882f,38.75266f,38.746506f,38.740356f,38.734215f,38.72808f,38.72195f,38.715824f,38.70971f,38.703594f,38.69749f,38.69139f,38.685295f,38.67921f,38.673126f,38.66705f,38.66098f};
class mydspSIG7 {
	
  private:
	
	int fmydspSIG7Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG7() {
		return 0;
	}
	int getNumOutputsmydspSIG7() {
		return 1;
	}
	
	void instanceInitmydspSIG7(int sample_rate) {
		fmydspSIG7Wave0_idx = 0;
	}
	
	void fillmydspSIG7(int count, float* table) {
		for (int i8 = 0; i8 < count; i8 = i8 + 1) {
			table[i8] = fmydspSIG7Wave0[fmydspSIG7Wave0_idx];
			fmydspSIG7Wave0_idx = (1 + fmydspSIG7Wave0_idx) % 2001;
		}
	}

};

static mydspSIG7* newmydspSIG7() { return (mydspSIG7*)new mydspSIG7(); }
static void deletemydspSIG7(mydspSIG7* dsp) { delete dsp; }

const static float fmydspSIG8Wave0[2001] = {127.202255f,127.14474f,127.08721f,127.02967f,126.97213f,126.91458f,126.857025f,126.79946f,126.74189f,126.68431f,126.626724f,126.56913f,126.511536f,126.453926f,126.39631f,126.338684f,126.28106f,126.22342f,126.16577f,126.10812f,126.05046f,125.9928f,125.93513f,125.87744f,125.819756f,125.76206f,125.70436f,125.64665f,125.588936f,125.53121f,125.47348f,125.41574f,125.357994f,125.30025f,125.242485f,125.18472f,125.126945f,125.06917f,125.01138f,124.95359f,124.89579f,124.83798f,124.78017f,124.72234f,124.66451f,124.60668f,124.548836f,124.49099f,124.43313f,124.37527f,124.3174f,124.25952f,124.20164f,124.143745f,124.08585f,124.02795f,123.97004f,123.91212f,123.854195f,123.796265f,123.73833f,123.68038f,123.62243f,123.564476f,123.50651f,123.44854f,123.390564f,123.33258f,123.27459f,123.21659f,123.158585f,123.10058f,123.04256f,122.984535f,122.926506f,122.86847f,122.810425f,122.75238f,122.69432f,122.63626f,122.57819f,122.52012f,122.462036f,122.403946f,122.34585f,122.28775f,122.229645f,122.17153f,122.11341f,122.05528f,121.997154f,121.93902f,121.88087f,121.822716f,121.764565f,121.7064f,121.64823f,121.59006f,121.531876f,121.47369f,121.41549f,121.35729f,121.29908f,121.24087f,121.182655f,121.12443f,121.0662f,121.00796f,120.949715f,120.89147f,120.83321f,120.774956f,120.71669f,120.65842f,120.600136f,120.541855f,120.48357f,120.42527f,120.36697f,120.308655f,120.25034f,120.192024f,120.1337f,120.07537f,120.01703f,119.95869f,119.900345f,119.84199f,119.78363f,119.725266f,119.66689f,119.60851f,119.55013f,119.491745f,119.43336f,119.374954f,119.31655f,119.25814f,119.19972f,119.141304f,119.08288f,119.024445f,118.96601f,118.90756f,118.84911f,118.790665f,118.7322f,118.67374f,118.61527f,118.55679f,118.498314f,118.43983f,118.38134f,118.32284f,118.264336f,118.20583f,118.147316f,118.0888f,118.03028f,117.97175f,117.913216f,117.85468f,117.796135f,117.73759f,117.67904f,117.620476f,117.56191f,117.50335f,117.44477f,117.38619f,117.327614f,117.26902f,117.21043f,117.15183f,117.09322f,117.03462f,116.976006f,116.91739f,116.858765f,116.80014f,116.74151f,116.68287f,116.62423f,116.56558f,116.50693f,116.44827f,116.38961f,116.33095f,116.27228f,116.2136f,116.15492f,116.09624f,116.03755f,115.97886f,115.92016f,115.86146f,115.80275f,115.74404f,115.685326f,115.62661f,115.56788f,115.509155f,115.450424f,115.391685f,115.33294f,115.27419f,115.21545f,115.15669f,115.09793f,115.03917f,114.9804f,114.92163f,114.862854f,114.80408f,114.74529f,114.6865f,114.62771f,114.56891f,114.51011f,114.45131f,114.3925f,114.33369f,114.27487f,114.21605f,114.15723f,114.0984f,114.039566f,113.98073f,113.92189f,113.863045f,113.8042f,113.745346f,113.68649f,113.62763f,113.56876f,113.5099f,113.45103f,113.39216f,113.33328f,113.2744f,113.215515f,113.156624f,113.09773f,113.03884f,112.979935f,112.921036f,112.86213f,112.80322f,112.74431f,112.685394f,112.62647f,112.56755f,112.50862f,112.44969f,112.39076f,112.331825f,112.27288f,112.21394f,112.15499f,112.09605f,112.037094f,111.978134f,111.919174f,111.860214f,111.801254f,111.74228f,111.68331f,111.62434f,111.56536f,111.50638f,111.447395f,111.38841f,111.32942f,111.27043f,111.21143f,111.152435f,111.09344f,111.03443f,110.975426f,110.91642f,110.85741f,110.79839f,110.73938f,110.68036f,110.62134f,110.56231f,110.50329f,110.44426f,110.38522f,110.32619f,110.26715f,110.208115f,110.14907f,110.09003f,110.030975f,109.97193f,109.91288f,109.85382f,109.79477f,109.73571f,109.67665f,109.617584f,109.55852f,109.49945f,109.440384f,109.38131f,109.322235f,109.26316f,109.20409f,109.145004f,109.08592f,109.02684f,108.96775f,108.90866f,108.84957f,108.79048f,108.73139f,108.672295f,108.6132f,108.5541f,108.494995f,108.4359f,108.37679f,108.31769f,108.25858f,108.19947f,108.14036f,108.081245f,108.02213f,107.96302f,107.9039f,107.84479f,107.78567f,107.72655f,107.66742f,107.6083f,107.54917f,107.49005f,107.43092f,107.371796f,107.31266f,107.25353f,107.1944f,107.13527f,107.07613f,107.017f,106.957855f,106.89872f,106.839584f,106.78044f,106.721306f,106.66216f,106.60302f,106.54388f,106.48473f,106.42558f,106.36644f,106.3073f,106.248146f,106.188995f,106.12985f,106.0707f,106.01155f,105.9524f,105.89325f,105.8341f,105.77495f,105.71579f,105.65664f,105.59749f,105.53833f,105.47918f,105.42002f,105.36087f,105.30171f,105.24256f,105.1834f,105.124245f,105.065094f,105.005936f,104.94678f,104.88762f,104.82847f,104.76931f,104.71015f,104.65099f,104.59184f,104.532684f,104.473526f,104.414375f,104.35522f,104.29606f,104.23691f,104.17775f,104.1186f,104.05944f,104.00029f,103.94114f,103.88198f,103.82283f,103.76368f,103.70453f,103.64538f,103.58623f,103.52708f,103.467926f,103.408775f,103.34963f,103.29048f,103.23134f,103.172195f,103.113045f,103.0539f,102.99476f,102.93562f,102.87648f,102.81734f,102.7582f,102.699066f,102.63992f,102.58079f,102.52165f,102.462524f,102.40339f,102.34426f,102.28513f,102.226006f,102.16688f,102.10775f,102.04862f,101.9895f,101.93038f,101.87126f,101.81214f,101.75303f,101.69391f,101.634796f,101.57568f,101.51658f,101.457466f,101.39836f,101.33926f,101.28015f,101.221054f,101.16196f,101.10286f,101.04376f,100.98467f,100.925575f,100.86649f,100.8074f,100.748314f,100.68923f,100.63016f,100.571075f,100.512f,100.45293f,100.39385f,100.334785f,100.27572f,100.21666f,100.15759f,100.09853f,100.03948f,99.98042f,99.92137f,99.86233f,99.803276f,99.74423f,99.685196f,99.62616f,99.56712f,99.50809f,99.44906f,99.39004f,99.33101f,99.271996f,99.212975f,99.15396f,99.09495f,99.03594f,98.97694f,98.91794f,98.85894f,98.79994f,98.74095f,98.68196f,98.62298f,98.563995f,98.50502f,98.446045f,98.38707f,98.3281f,98.26914f,98.21018f,98.15122f,98.09227f,98.033325f,97.97438f,97.915436f,97.8565f,97.79757f,97.73864f,97.67971f,97.62079f,97.561874f,97.50296f,97.44405f,97.38515f,97.32625f,97.26735f,97.20846f,97.14957f,97.09068f,97.03181f,96.97293f,96.91406f,96.855194f,96.79633f,96.73748f,96.67863f,96.61978f,96.560936f,96.5021f,96.44327f,96.38444f,96.325615f,96.26679f,96.207985f,96.14917f,96.09037f,96.03157f,95.97278f,95.913994f,95.85521f,95.79643f,95.737656f,95.67889f,95.620125f,95.56137f,95.50262f,95.44388f,95.38513f,95.3264f,95.26767f,95.20895f,95.15023f,95.091515f,95.032814f,94.97411f,94.91541f,94.85673f,94.79804f,94.739365f,94.680695f,94.62203f,94.56337f,94.50472f,94.446075f,94.387436f,94.3288f,94.27017f,94.21155f,94.15293f,94.09432f,94.03572f,93.97713f,93.91853f,93.859955f,93.80138f,93.742805f,93.68424f,93.62569f,93.56714f,93.5086f,93.45006f,93.39153f,93.33301f,93.27449f,93.21599f,93.157486f,93.09899f,93.040504f,92.982025f,92.92355f,92.86509f,92.80663f,92.74818f,92.689735f,92.6313f,92.572876f,92.51445f,92.45604f,92.39764f,92.33923f,92.280846f,92.222466f,92.16409f,92.10573f,92.04736f,91.98901f,91.93067f,91.87234f,91.81401f,91.75569f,91.69738f,91.639084f,91.58079f,91.5225f,91.464226f,91.40596f,91.347694f,91.289444f,91.2312f,91.172966f,91.11474f,91.056526f,90.998314f,90.94012f,90.88192f,90.82374f,90.765564f,90.707405f,90.64925f,90.5911f,90.53297f,90.47484f,90.41672f,90.358604f,90.30051f,90.24242f,90.18433f,90.12626f,90.0682f,90.01015f,89.9521f,89.894066f,89.836044f,89.77803f,89.720024f,89.662025f,89.60404f,89.54607f,89.4881f,89.430145f,89.3722f,89.31426f,89.25634f,89.198425f,89.14052f,89.08263f,89.02474f,88.966866f,88.909004f,88.85115f,88.79331f,88.73548f,88.67766f,88.61985f,88.56205f,88.504265f,88.44649f,88.388725f,88.33096f,88.273224f,88.21549f,88.15777f,88.10006f,88.04236f,87.98467f,87.926994f,87.86933f,87.81168f,87.754036f,87.69641f,87.638794f,87.581184f,87.52359f,87.46601f,87.40844f,87.35088f,87.293335f,87.2358f,87.17828f,87.12077f,87.06328f,87.0058f,86.948326f,86.89087f,86.83342f,86.77599f,86.718575f,86.66116f,86.603775f,86.546394f,86.48903f,86.43167f,86.37433f,86.31701f,86.25969f,86.20239f,86.14511f,86.08784f,86.03058f,85.973335f,85.91611f,85.858894f,85.8017f,85.74451f,85.68734f,85.63018f,85.573044f,85.515915f,85.45881f,85.40171f,85.34463f,85.28757f,85.230515f,85.173485f,85.11647f,85.05946f,85.00248f,84.94551f,84.888565f,84.83163f,84.77471f,84.71781f,84.66093f,84.60406f,84.54721f,84.49038f,84.43357f,84.37678f,84.32f,84.263245f,84.206505f,84.14979f,84.09309f,84.03641f,83.979744f,83.9231f,83.866486f,83.80989f,83.75331f,83.696754f,83.64022f,83.5837f,83.527214f,83.47075f,83.4143f,83.35788f,83.30148f,83.2451f,83.18876f,83.13243f,83.076126f,83.01985f,82.96361f,82.90739f,82.85119f,82.79502f,82.738884f,82.68277f,82.62669f,82.57064f,82.51462f,82.458626f,82.40267f,82.34674f,82.29085f,82.234985f,82.17916f,82.12337f,82.06761f,82.011894f,81.956215f,81.900566f,81.84496f,81.7894f,81.73387f,81.67839f,81.622955f,81.56756f,81.51221f,81.4569f,81.40164f,81.346436f,81.291275f,81.23617f,81.18111f,81.12611f,81.07116f,81.016266f,80.96143f,80.906654f,80.85194f,80.79729f,80.7427f,80.68817f,80.63372f,80.57933f,80.52502f,80.47077f,80.4166f,80.36251f,80.3085f,80.25457f,80.20072f,80.146965f,80.09329f,80.0397f,79.98621f,79.93282f,79.879524f,79.82633f,79.77324f,79.720245f,79.66737f,79.6146f,79.56195f,79.50942f,79.45701f,79.404724f,79.35256f,79.30053f,79.248634f,79.19688f,79.14526f,79.09379f,79.042465f,78.991295f,78.94028f,78.88942f,78.83873f,78.7882f,78.73785f,78.68767f,78.637665f,78.587845f,78.538216f,78.48878f,78.43954f,78.39049f,78.34165f,78.293015f,78.2446f,78.196396f,78.14841f,78.10065f,78.05312f,78.00582f,77.95876f,77.91194f,77.86537f,77.819046f,77.77297f,77.72716f,77.6816f,77.63631f,77.591286f,77.54654f,77.50206f,77.45786f,77.41395f,77.370316f,77.32697f,77.28392f,77.241165f,77.1987f,77.15654f,77.11468f,77.07312f,77.03187f,76.99093f,76.950294f,76.90997f,76.869965f,76.83028f,76.7909f,76.75185f,76.713104f,76.67468f,76.63658f,76.59881f,76.56135f,76.524216f,76.487404f,76.450905f,76.41474f,76.37889f,76.34336f,76.30815f,76.27327f,76.2387f,76.204445f,76.17052f,76.136894f,76.10359f,76.0706f,76.037926f,76.005554f,75.973495f,75.94175f,75.9103f,75.87916f,75.84831f,75.817764f,75.78752f,75.75757f,75.727905f,75.69854f,75.66946f,75.640656f,75.612144f,75.58391f,75.55595f,75.52827f,75.500854f,75.47372f,75.446846f,75.420235f,75.39389f,75.367805f,75.34197f,75.31639f,75.29107f,75.26599f,75.24116f,75.21656f,75.19221f,75.1681f,75.14422f,75.12057f,75.097145f,75.07395f,75.05098f,75.02823f,75.00569f,74.983376f,74.961266f,74.93937f,74.91768f,74.896194f,74.874916f,74.85383f,74.83295f,74.812256f,74.791756f,74.77145f,74.75132f,74.731384f,74.711624f,74.69205f,74.67265f,74.65343f,74.63438f,74.6155f,74.59679f,74.57825f,74.55986f,74.54164f,74.52359f,74.505684f,74.48794f,74.47035f,74.45291f,74.43562f,74.41848f,74.40148f,74.384636f,74.36792f,74.35136f,74.33492f,74.31863f,74.30247f,74.286446f,74.27055f,74.25478f,74.23915f,74.22364f,74.20825f,74.19299f,74.17785f,74.162834f,74.147934f,74.13315f,74.11848f,74.10393f,74.089485f,74.07516f,74.060936f,74.04682f,74.032814f,74.01892f,74.00513f,73.99143f,73.977844f,73.96436f,73.95097f,73.93768f,73.92449f,73.91139f,73.89839f,73.88548f,73.872665f,73.85995f,73.84731f,73.83477f,73.82231f,73.809944f,73.79766f,73.78546f,73.773346f,73.761314f,73.74937f,73.7375f,73.725716f,73.71401f,73.702385f,73.690834f,73.67936f,73.66796f,73.65663f,73.645386f,73.63421f,73.62311f,73.612076f,73.60111f,73.59022f,73.5794f,73.56865f,73.55796f,73.54734f,73.53679f,73.526306f,73.515884f,73.50552f,73.49523f,73.485f,73.47483f,73.46472f,73.45467f,73.444695f,73.43476f,73.4249f,73.415085f,73.405334f,73.39564f,73.386f,73.37642f,73.3669f,73.35742f,73.34801f,73.33864f,73.32933f,73.320076f,73.31087f,73.30171f,73.29261f,73.283554f,73.27455f,73.2656f,73.2567f,73.24784f,73.23904f,73.23028f,73.221565f,73.2129f,73.20428f,73.1957f,73.18718f,73.178696f,73.17026f,73.161865f,73.15351f,73.1452f,73.13694f,73.128716f,73.12054f,73.112404f,73.1043f,73.09625f,73.088234f,73.08026f,73.07233f,73.06443f,73.05657f,73.04876f,73.04098f,73.03324f,73.025536f,73.01787f,73.01024f,73.00265f,72.995094f,72.98758f,72.980095f,72.97265f,72.96523f,72.957855f,72.950516f,72.94321f,72.93593f,72.92869f,72.921486f,72.91431f,72.907166f,72.90006f,72.89298f,72.88594f,72.87893f,72.87195f,72.865f,72.85808f,72.85119f,72.84433f,72.83751f,72.83071f,72.823944f,72.81721f,72.81049f,72.80382f,72.797165f,72.79054f,72.78395f,72.77739f,72.77085f,72.76434f,72.75786f,72.7514f,72.74498f,72.73858f,72.7322f,72.72585f,72.719536f,72.713234f,72.70697f,72.70072f,72.6945f,72.68832f,72.682144f,72.676f,72.66988f,72.66379f,72.65772f,72.65167f,72.64565f,72.639656f,72.63368f,72.62773f,72.6218f,72.615906f,72.61002f,72.604164f,72.59833f,72.592514f,72.58672f,72.580956f,72.57521f,72.56948f,72.56378f,72.5581f,72.55244f,72.5468f,72.54118f,72.53558f,72.53001f,72.52445f,72.51892f,72.513405f,72.50791f,72.50243f,72.49698f,72.49155f,72.48613f,72.480736f,72.47536f,72.47f,72.46466f,72.45934f,72.45405f,72.44876f,72.443504f,72.438255f,72.43303f,72.427826f,72.42264f,72.417465f,72.412315f,72.40717f,72.40206f,72.39696f,72.39188f,72.38681f,72.38176f,72.37673f,72.37172f,72.36672f,72.36174f,72.35678f,72.35183f,72.3469f,72.34199f,72.33709f,72.33221f,72.32734f,72.322495f,72.31766f,72.31284f,72.30804f,72.30325f,72.298485f,72.293724f,72.28899f,72.28426f,72.27955f,72.27486f,72.27017f,72.26551f,72.26086f,72.256226f,72.2516f,72.246994f,72.2424f,72.23782f,72.23326f,72.228714f,72.224174f,72.21966f,72.21515f,72.210655f,72.20618f,72.201706f,72.19726f,72.19282f,72.18839f,72.183975f,72.17958f,72.17519f,72.17082f,72.16646f,72.16212f,72.15778f,72.15346f,72.149155f,72.14485f,72.14057f,72.1363f,72.13204f,72.1278f,72.123566f,72.11935f,72.115135f,72.11094f,72.10676f,72.102585f,72.09842f,72.09428f,72.09014f,72.08601f,72.0819f,72.0778f,72.07371f,72.069626f,72.06556f,72.06151f,72.057465f,72.05343f,72.04941f,72.045395f,72.0414f,72.03741f,72.03343f,72.029465f,72.025505f,72.02156f,72.017624f,72.0137f,72.00979f,72.00589f,72.002f,71.998116f,71.99424f,71.99038f,71.986534f,71.98269f,71.97886f,71.975044f,71.97123f,71.96743f,71.963646f,71.95986f,71.95609f,71.95233f,71.948586f,71.94484f,71.94111f,71.93739f,71.93368f,71.92998f,71.926285f,71.9226f,71.91893f,71.91526f,71.911606f,71.90796f,71.90433f,71.900696f,71.89708f,71.89347f,71.88987f,71.886284f,71.8827f,71.87913f,71.875565f,71.87201f,71.86846f,71.86492f,71.8614f,71.85787f,71.85436f,71.85086f,71.84737f,71.84388f,71.8404f,71.83693f,71.83347f,71.83002f,71.82658f,71.82314f,71.81971f,71.81629f,71.81288f,71.80948f,71.80608f,71.8027f,71.79932f,71.79595f,71.79259f,71.78923f,71.78588f,71.78255f,71.77921f,71.77589f,71.772575f,71.769264f,71.76597f,71.76267f,71.759384f,71.75611f,71.75284f,71.74958f,71.74632f,71.74307f,71.73984f,71.7366f,71.733376f,71.730156f,71.726944f,71.72374f,71.72054f,71.717354f,71.71417f,71.711f,71.70783f,71.70467f,71.701515f,71.69837f,71.69523f,71.6921f,71.68897f,71.68585f,71.68274f,71.679634f,71.67654f,71.67345f,71.67036f,71.66728f,71.66421f,71.66114f,71.65808f,71.65503f,71.651985f,71.64895f,71.64591f,71.64289f,71.63987f,71.636856f,71.63385f,71.63085f,71.62785f,71.62487f,71.62189f,71.61891f,71.615944f,71.61298f,71.61002f,71.60707f,71.60413f,71.6012f,71.59826f,71.59534f,71.592415f,71.5895f,71.58659f,71.583694f,71.580795f,71.57791f,71.57503f,71.57214f,71.569275f,71.56641f,71.563545f,71.56069f,71.557846f,71.555f,71.55216f,71.54933f,71.54651f,71.543686f,71.54087f,71.53806f,71.53526f,71.53246f,71.52967f,71.526886f,71.5241f,71.521324f,71.518555f,71.51579f,71.51303f,71.51028f,71.50753f,71.50478f,71.50205f,71.49931f,71.49659f,71.493866f,71.49115f,71.48844f,71.48573f,71.48303f,71.48034f,71.477646f,71.47496f,71.47228f,71.469604f,71.466934f,71.46427f,71.46161f,71.458954f,71.45631f,71.45366f,71.45102f,71.44839f,71.445755f,71.44313f,71.44051f,71.4379f,71.43529f,71.43268f,71.43008f,71.42748f,71.4249f,71.42231f,71.41972f,71.41715f,71.41457f,71.41201f,71.40945f,71.40689f,71.404335f,71.40179f,71.39925f,71.396706f,71.39417f,71.39164f,71.389114f,71.3866f,71.38408f,71.38156f,71.37906f,71.37656f,71.374054f,71.37156f,71.36907f,71.366585f,71.364105f,71.36163f,71.35916f,71.35669f,71.35423f,71.35177f,71.34932f,71.34686f,71.34442f,71.34198f,71.33954f,71.337105f,71.33468f,71.33225f,71.329834f,71.327415f,71.325005f,71.3226f,71.3202f,71.317795f,71.3154f,71.31301f,71.31062f,71.30824f,71.30586f,71.30348f,71.30112f,71.298744f,71.29639f,71.29403f,71.29167f,71.28932f,71.28697f,71.28463f,71.282295f,71.27995f,71.277626f,71.2753f,71.27297f,71.27065f,71.26834f,71.26603f,71.26372f,71.26141f,71.25912f,71.25682f,71.254524f,71.252235f,71.249954f,71.24767f,71.24539f,71.24312f,71.24085f,71.23859f,71.23632f,71.23406f,71.231804f,71.22955f,71.22731f,71.22506f,71.222824f,71.22058f,71.21835f,71.21612f,71.2139f,71.21167f,71.20945f,71.20724f,71.205025f,71.20281f,71.20061f,71.19841f,71.19621f,71.194016f,71.191826f,71.18964f,71.187454f,71.18527f,71.1831f,71.18092f,71.17875f,71.17658f,71.174416f,71.17226f,71.170105f,71.167946f,71.165794f,71.16365f,71.16151f,71.15936f,71.15723f,71.1551f,71.15296f,71.15084f,71.14871f,71.14659f,71.14448f,71.142365f,71.14025f,71.138145f,71.13604f,71.133934f,71.131836f,71.129745f,71.127655f,71.125565f,71.12348f,71.1214f,71.119316f,71.11724f,71.115166f,71.1131f,71.11103f,71.10897f,71.10691f,71.10485f,71.1028f,71.100746f,71.098694f,71.09665f,71.094604f,71.09257f,71.09053f,71.0885f,71.08647f,71.08444f,71.08242f,71.0804f,71.07838f,71.07636f,71.07435f,71.07234f,71.070335f,71.06833f,71.06633f,71.06433f,71.06234f,71.06034f,71.05836f,71.056366f,71.05438f,71.05241f,71.05043f,71.048454f,71.04648f,71.04451f,71.04254f,71.04058f,71.03862f,71.03666f,71.034706f,71.03275f,71.0308f,71.028854f,71.02691f,71.02497f,71.02303f,71.021095f,71.019165f,71.017235f,71.015305f,71.01338f,71.01146f,71.00954f,71.00762f,71.00571f,71.00379f,71.001884f,70.99998f,70.99807f,70.99617f,70.99427f,70.99238f,70.990486f,70.988594f,70.9867f,70.98482f,70.98293f,70.98106f,70.97918f,70.9773f,70.975426f,70.97356f,70.971695f,70.969826f,70.967964f,70.9661f,70.96425f,70.96239f,70.96054f,70.95869f,70.95684f,70.954994f,70.953156f,70.95131f,70.94948f,70.94764f,70.94581f,70.94398f,70.94215f,70.94032f,70.9385f,70.93668f,70.93486f,70.933044f,70.93124f,70.92942f,70.92761f,70.925804f,70.924f,70.9222f,70.9204f,70.9186f,70.91681f,70.915016f,70.91323f,70.911446f,70.90966f,70.907875f,70.9061f,70.90431f,70.90254f,70.900764f,70.898994f,70.897224f,70.89546f,70.89369f,70.89194f,70.890175f,70.88841f,70.88666f,70.88491f,70.883156f,70.88141f,70.87966f,70.877914f,70.876175f,70.874435f,70.872696f,70.870964f,70.86923f,70.8675f,70.86577f,70.864044f,70.86232f,70.860596f,70.85888f,70.85716f,70.855446f,70.85373f,70.85202f,70.85031f,70.8486f,70.8469f,70.84519f,70.8435f,70.8418f,70.8401f,70.8384f,70.836716f,70.83502f,70.833336f,70.83165f,70.82996f,70.828285f,70.8266f,70.82493f,70.82325f,70.82158f,70.8199f,70.81824f,70.81657f,70.8149f,70.81324f,70.81158f,70.80991f,70.80826f,70.8066f,70.80495f,70.8033f,70.80165f,70.8f,70.798355f,70.796715f,70.795074f,70.793434f,70.791794f,70.79016f,70.78853f,70.786896f,70.78526f,70.78364f,70.78201f,70.78039f,70.77876f,70.777145f,70.77553f,70.77391f,70.77229f,70.77068f,70.76907f,70.76746f,70.76586f,70.76425f,70.76265f,70.76105f,70.75945f,70.75785f,70.756256f,70.75466f,70.753075f,70.75148f,70.74989f,70.74831f,70.74673f,70.74514f,70.74356f,70.74198f,70.7404f,70.73883f,70.73726f,70.73569f,70.734116f,70.73255f,70.73098f,70.729416f,70.72786f,70.726295f,70.72474f,70.72318f,70.72163f,70.72007f,70.71852f,70.71697f,70.71542f,70.713875f,70.712326f,70.710785f,70.70924f,70.7077f,70.70617f,70.70463f,70.703094f,70.70157f,70.700035f,70.6985f,70.696976f,70.69545f,70.69393f,70.692406f,70.69089f,70.68937f,70.68785f,70.68633f,70.68482f,70.68331f,70.6818f,70.68029f,70.67878f,70.67728f,70.67577f,70.67427f,70.67277f,70.67127f,70.66978f,70.66828f,70.66679f,70.66529f,70.6638f,70.662315f,70.66083f,70.65934f,70.65786f,70.65637f,70.65489f,70.65341f,70.65194f,70.65046f,70.64899f,70.647514f,70.64604f,70.64457f,70.643105f,70.64164f,70.640175f,70.63871f,70.637245f,70.63579f,70.63433f,70.63287f,70.63142f,70.62997f,70.62851f,70.62706f,70.62561f,70.62416f,70.62272f,70.62127f,70.61983f,70.618385f,70.61695f,70.61551f,70.614075f,70.61264f,70.611206f,70.60977f,70.60834f,70.60691f,70.605484f,70.60406f,70.60263f,70.60121f,70.599785f,70.598366f,70.59695f,70.59553f,70.59412f,70.5927f,70.591286f,70.589874f,70.58846f,70.58706f,70.58565f,70.58424f,70.58284f,70.581436f,70.58003f,70.578636f,70.57724f,70.57584f,70.57445f,70.57305f,70.571655f,70.57027f,70.56888f,70.56749f,70.5661f,70.56471f,70.56333f,70.56195f,70.56057f,70.55919f,70.55781f,70.556435f,70.55505f,70.55368f,70.55231f,70.55094f,70.54957f,70.5482f,70.54683f,70.54546f,70.5441f,70.54274f,70.541374f,70.540016f,70.53866f,70.5373f,70.53594f,70.534584f,70.53323f,70.53188f,70.53053f,70.52918f,70.52783f,70.52649f,70.52514f,70.523796f,70.52245f,70.52111f,70.51977f,70.51843f,70.5171f,70.515755f,70.51442f,70.51309f,70.51176f,70.51043f,70.509094f,70.50777f,70.50644f,70.50511f,70.50379f,70.502464f,70.501144f,70.499825f,70.498505f,70.497185f,70.49587f,70.49455f,70.49324f,70.49193f,70.490616f,70.4893f,70.488f,70.48669f,70.48538f,70.48408f,70.48277f,70.48147f,70.48017f,70.47887f,70.47757f,70.47627f,70.474976f,70.47368f,70.47238f,70.47109f,70.4698f,70.46851f,70.467224f,70.465935f,70.464645f,70.46336f};
class mydspSIG8 {
	
  private:
	
	int fmydspSIG8Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG8() {
		return 0;
	}
	int getNumOutputsmydspSIG8() {
		return 1;
	}
	
	void instanceInitmydspSIG8(int sample_rate) {
		fmydspSIG8Wave0_idx = 0;
	}
	
	void fillmydspSIG8(int count, float* table) {
		for (int i9 = 0; i9 < count; i9 = i9 + 1) {
			table[i9] = fmydspSIG8Wave0[fmydspSIG8Wave0_idx];
			fmydspSIG8Wave0_idx = (1 + fmydspSIG8Wave0_idx) % 2001;
		}
	}

};

static mydspSIG8* newmydspSIG8() { return (mydspSIG8*)new mydspSIG8(); }
static void deletemydspSIG8(mydspSIG8* dsp) { delete dsp; }

const static float fmydspSIG9Wave0[2001] = {246.23625f,246.22417f,246.21202f,246.19981f,246.18753f,246.17517f,246.16275f,246.15027f,246.13771f,246.12508f,246.11237f,246.0996f,246.08676f,246.07384f,246.06085f,246.04779f,246.03465f,246.02145f,246.00816f,245.99481f,245.98138f,245.96788f,245.9543f,245.94064f,245.92691f,245.9131f,245.8992f,245.88524f,245.8712f,245.85707f,245.84286f,245.82858f,245.81422f,245.79979f,245.78526f,245.77066f,245.75597f,245.7412f,245.72635f,245.71141f,245.6964f,245.68129f,245.66609f,245.65082f,245.63547f,245.62003f,245.60449f,245.58887f,245.57315f,245.55736f,245.54147f,245.5255f,245.50945f,245.49329f,245.47704f,245.46071f,245.44427f,245.42775f,245.41113f,245.39444f,245.37762f,245.36073f,245.34375f,245.32666f,245.30948f,245.2922f,245.27483f,245.25735f,245.23978f,245.2221f,245.20433f,245.18646f,245.1685f,245.15042f,245.13225f,245.11398f,245.0956f,245.07712f,245.05853f,245.03984f,245.02104f,245.00214f,244.98314f,244.96402f,244.9448f,244.92546f,244.90602f,244.88647f,244.86682f,244.84705f,244.82716f,244.80717f,244.78708f,244.76685f,244.74652f,244.72607f,244.70552f,244.68483f,244.66405f,244.64313f,244.6221f,244.60095f,244.5797f,244.5583f,244.5368f,244.51518f,244.49342f,244.47156f,244.44957f,244.42744f,244.40521f,244.38284f,244.36035f,244.33772f,244.31499f,244.29211f,244.2691f,244.24597f,244.22272f,244.19933f,244.1758f,244.15215f,244.12836f,244.10443f,244.08037f,244.05618f,244.03185f,244.00739f,243.98279f,243.95804f,243.93317f,243.90814f,243.88298f,243.85768f,243.83224f,243.80666f,243.78093f,243.75505f,243.72903f,243.70287f,243.67656f,243.6501f,243.62349f,243.59674f,243.56982f,243.54277f,243.51556f,243.48819f,243.46068f,243.43301f,243.40518f,243.37721f,243.34908f,243.32077f,243.29233f,243.2637f,243.23494f,243.20601f,243.17691f,243.14764f,243.11823f,243.08864f,243.05888f,243.02898f,242.99889f,242.96863f,242.9382f,242.90761f,242.87685f,242.84592f,242.8148f,242.78352f,242.75208f,242.72044f,242.68863f,242.65665f,242.62448f,242.59213f,242.55962f,242.5269f,242.49402f,242.46094f,242.42769f,242.39426f,242.36063f,242.32681f,242.2928f,242.25862f,242.22423f,242.18967f,242.15489f,242.11993f,242.0848f,242.04944f,242.0139f,241.97815f,241.94221f,241.90607f,241.86974f,241.83319f,241.79645f,241.7595f,241.72235f,241.68498f,241.64742f,241.60965f,241.57167f,241.53348f,241.49507f,241.45645f,241.41762f,241.37857f,241.33931f,241.29984f,241.26015f,241.22023f,241.18008f,241.13974f,241.09915f,241.05835f,241.01732f,240.97607f,240.93459f,240.89287f,240.85094f,240.80876f,240.76636f,240.72372f,240.68085f,240.63774f,240.5944f,240.55083f,240.507f,240.46294f,240.41864f,240.3741f,240.3293f,240.28427f,240.23898f,240.19345f,240.14767f,240.10164f,240.05536f,240.00883f,239.96204f,239.915f,239.86769f,239.82013f,239.77231f,239.72423f,239.67589f,239.62729f,239.57841f,239.52927f,239.47987f,239.43019f,239.38025f,239.33003f,239.27954f,239.22879f,239.17775f,239.12643f,239.07484f,239.02296f,238.97081f,238.91838f,238.86566f,238.81265f,238.75935f,238.70578f,238.6519f,238.59775f,238.54329f,238.48854f,238.4335f,238.37816f,238.32253f,238.26659f,238.21036f,238.15381f,238.09697f,238.03983f,237.98236f,237.92459f,237.86652f,237.80814f,237.74944f,237.69043f,237.63109f,237.57144f,237.51147f,237.45119f,237.39058f,237.32965f,237.26839f,237.20682f,237.1449f,237.08266f,237.02008f,236.95718f,236.89394f,236.83037f,236.76645f,236.7022f,236.6376f,236.57266f,236.50739f,236.44176f,236.37578f,236.30946f,236.2428f,236.17577f,236.10838f,236.04065f,235.97256f,235.90411f,235.83531f,235.76614f,235.69661f,235.62671f,235.55644f,235.48581f,235.4148f,235.34343f,235.27167f,235.19955f,235.12704f,235.05417f,234.98091f,234.90726f,234.83324f,234.75882f,234.68402f,234.60884f,234.53326f,234.45729f,234.38092f,234.30415f,234.227f,234.14944f,234.07149f,233.99312f,233.91435f,233.83519f,233.7556f,233.67561f,233.59521f,233.51439f,233.43317f,233.35152f,233.26945f,233.18697f,233.10405f,233.02072f,232.93697f,232.85277f,232.76816f,232.6831f,232.59763f,232.5117f,232.42535f,232.33856f,232.25133f,232.16365f,232.07553f,231.98697f,231.89796f,231.8085f,231.71858f,231.62822f,231.53738f,231.4461f,231.35437f,231.26218f,231.16951f,231.07639f,230.98279f,230.88873f,230.79419f,230.69919f,230.60371f,230.50777f,230.41133f,230.31442f,230.21703f,230.11916f,230.02078f,229.92194f,229.8226f,229.72278f,229.62245f,229.52164f,229.42033f,229.31851f,229.2162f,229.11339f,229.01007f,228.90625f,228.80193f,228.69708f,228.59174f,228.48587f,228.37949f,228.27258f,228.16516f,228.05722f,227.94876f,227.83977f,227.73026f,227.62021f,227.50963f,227.39853f,227.28688f,227.1747f,227.06198f,226.94873f,226.83493f,226.72058f,226.6057f,226.49025f,226.37427f,226.25772f,226.14062f,226.02298f,225.90477f,225.78601f,225.66667f,225.54678f,225.42633f,225.30531f,225.18372f,225.06155f,224.93881f,224.8155f,224.69162f,224.56715f,224.44212f,224.31648f,224.19028f,224.06349f,223.9361f,223.80814f,223.67957f,223.55042f,223.42065f,223.29031f,223.15936f,223.02782f,222.89566f,222.76291f,222.62956f,222.49559f,222.36101f,222.22583f,222.09003f,221.95361f,221.81657f,221.67892f,221.54065f,221.40176f,221.26224f,221.1221f,220.98132f,220.83994f,220.6979f,220.55524f,220.41194f,220.268f,220.12344f,219.97823f,219.83238f,219.68588f,219.53874f,219.39096f,219.24254f,219.09344f,218.94371f,218.79333f,218.64229f,218.4906f,218.33824f,218.18523f,218.03156f,217.87721f,217.72221f,217.56654f,217.41022f,217.2532f,217.09554f,216.9372f,216.77817f,216.61848f,216.45811f,216.29706f,216.13533f,215.97292f,215.80983f,215.64606f,215.4816f,215.31645f,215.15062f,214.98409f,214.81686f,214.64896f,214.48036f,214.31107f,214.14107f,213.97038f,213.79898f,213.62689f,213.4541f,213.28061f,213.1064f,212.9315f,212.75589f,212.57956f,212.40253f,212.2248f,212.04634f,211.86717f,211.6873f,211.5067f,211.3254f,211.14336f,210.96062f,210.77715f,210.59297f,210.40805f,210.22243f,210.03607f,209.849f,209.6612f,209.47266f,209.2834f,209.09341f,208.9027f,208.71124f,208.51907f,208.32616f,208.13252f,207.93814f,207.74303f,207.54718f,207.3506f,207.15327f,206.95523f,206.75642f,206.5569f,206.35663f,206.15561f,205.95386f,205.75136f,205.54813f,205.34415f,205.13942f,204.93394f,204.72774f,204.52078f,204.31308f,204.10463f,203.89545f,203.6855f,203.47482f,203.26338f,203.05121f,202.83827f,202.6246f,202.41019f,202.195f,201.97908f,201.76242f,201.545f,201.32683f,201.10791f,200.88824f,200.66783f,200.44666f,200.22475f,200.00208f,199.77866f,199.55449f,199.32957f,199.10391f,198.8775f,198.65034f,198.42242f,198.19377f,197.96437f,197.7342f,197.5033f,197.27165f,197.03925f,196.8061f,196.57222f,196.33757f,196.10219f,195.86606f,195.62918f,195.39157f,195.1532f,194.9141f,194.67426f,194.43365f,194.19232f,193.95026f,193.70744f,193.4639f,193.2196f,192.97458f,192.7288f,192.48232f,192.23508f,191.9871f,191.7384f,191.48897f,191.23882f,190.98792f,190.7363f,190.48393f,190.23087f,189.97705f,189.72253f,189.46727f,189.2113f,188.9546f,188.69719f,188.43906f,188.1802f,187.92064f,187.66037f,187.39937f,187.13766f,186.87526f,186.61214f,186.34831f,186.08379f,185.81854f,185.55261f,185.28598f,185.01865f,184.75061f,184.48189f,184.21248f,183.94237f,183.67157f,183.40009f,183.12791f,182.85506f,182.58153f,182.30731f,182.03242f,181.75687f,181.48062f,181.20372f,180.92615f,180.6479f,180.36899f,180.08943f,179.8092f,179.52832f,179.24678f,178.96458f,178.68175f,178.39827f,178.11414f,177.82936f,177.54395f,177.2579f,176.97122f,176.68391f,176.39597f,176.10739f,175.8182f,175.5284f,175.23796f,174.94693f,174.65527f,174.363f,174.07014f,173.77667f,173.4826f,173.18793f,172.89267f,172.59683f,172.30038f,172.00337f,171.70576f,171.4076f,171.10884f,170.80952f,170.50964f,170.20918f,169.90817f,169.60661f,169.30449f,169.00182f,168.69861f,168.39485f,168.09056f,167.78574f,167.48038f,167.17448f,166.86807f,166.56116f,166.25371f,165.94576f,165.6373f,165.32834f,165.01888f,164.70891f,164.39847f,164.08754f,163.77612f,163.46423f,163.15186f,162.83902f,162.52573f,162.21196f,161.89775f,161.58308f,161.26797f,160.95241f,160.63641f,160.31999f,160.00314f,159.68587f,159.36818f,159.05008f,158.73157f,158.41266f,158.09334f,157.77364f,157.45355f,157.13307f,156.81223f,156.49101f,156.16942f,155.84747f,155.52518f,155.20253f,154.87955f,154.55621f,154.23256f,153.90857f,153.58427f,153.25966f,152.93474f,152.60951f,152.284f,151.95819f,151.63211f,151.30574f,150.97913f,150.65224f,150.32509f,149.9977f,149.67007f,149.34221f,149.01411f,148.6858f,148.3573f,148.02858f,147.69966f,147.37056f,147.04128f,146.71182f,146.3822f,146.05244f,145.72252f,145.39247f,145.06229f,144.732f,144.40158f,144.07108f,143.74046f,143.40979f,143.07903f,142.74821f,142.41734f,142.08644f,141.75551f,141.42455f,141.09358f,140.76262f,140.43167f,140.10074f,139.76985f,139.43903f,139.10825f,138.77756f,138.44695f,138.11644f,137.78606f,137.4558f,137.12567f,136.7957f,136.46591f,136.1363f,135.80692f,135.47772f,135.14877f,134.82007f,134.49164f,134.16348f,133.83563f,133.5081f,133.1809f,132.85405f,132.52759f,132.2015f,131.87584f,131.5506f,131.22581f,130.90149f,130.57767f,130.25436f,129.93158f,129.60938f,129.28773f,128.9667f,128.64629f,128.32654f,128.00745f,127.68906f,127.37139f,127.054474f,126.738335f,126.42299f,126.10847f,125.79479f,125.482f,125.17011f,124.85915f,124.54916f,124.24014f,123.93215f,123.6252f,123.31931f,123.01453f,122.71088f,122.40838f,122.10706f,121.80696f,121.50811f,121.210526f,120.91424f,120.619286f,120.32568f,120.03348f,119.742676f,119.45332f,119.165436f,118.87904f,118.59418f,118.31086f,118.02912f,117.748985f,117.47048f,117.19362f,116.91844f,116.644966f,116.37321f,116.1032f,115.83495f,115.568504f,115.303856f,115.04104f,114.78007f,114.52096f,114.26373f,114.0084f,113.75498f,113.50349f,113.25394f,113.00633f,112.76069f,112.51702f,112.27534f,112.03564f,111.79794f,111.56225f,111.32856f,111.096886f,110.86723f,110.6396f,110.41399f,110.19039f,109.96882f,109.74927f,109.53174f,109.316216f,109.10271f,108.89121f,108.68171f,108.474205f,108.268684f,108.06514f,107.86358f,107.66397f,107.46632f,107.2706f,107.07682f,106.88496f,106.695f,106.506935f,106.320755f,106.136444f,105.95398f,105.77336f,105.59457f,105.41757f,105.24238f,105.06896f,104.89731f,104.727394f,104.55921f,104.392746f,104.22797f,104.06487f,103.903435f,103.74364f,103.58547f,103.42891f,103.27394f,103.120544f,102.968704f,102.818405f,102.669624f,102.52234f,102.37655f,102.232216f,102.08934f,101.94789f,101.80785f,101.66922f,101.53196f,101.39607f,101.26152f,101.1283f,100.99639f,100.865776f,100.73644f,100.60837f,100.481544f,100.35595f,100.23156f,100.108376f,99.986374f,99.86553f,99.74585f,99.6273f,99.509865f,99.39355f,99.27831f,99.16416f,99.05106f,98.939026f,98.82802f,98.718025f,98.60905f,98.50106f,98.39405f,98.28802f,98.18293f,98.07879f,97.97558f,97.87328f,97.77189f,97.671394f,97.57178f,97.47303f,97.37514f,97.2781f,97.181885f,97.0865f,96.991936f,96.89816f,96.80519f,96.713f,96.621574f,96.530914f,96.44101f,96.35184f,96.263405f,96.17569f,96.08869f,96.0024f,95.9168f,95.83189f,95.74765f,95.664085f,95.58118f,95.498924f,95.41731f,95.33634f,95.256f,95.17627f,95.09715f,95.01864f,94.94073f,94.8634f,94.78666f,94.710495f,94.634895f,94.55986f,94.485374f,94.41144f,94.33804f,94.26518f,94.19285f,94.12103f,94.049736f,93.97895f,93.90867f,93.83888f,93.769585f,93.700775f,93.63244f,93.56458f,93.4972f,93.430275f,93.36381f,93.29779f,93.23222f,93.1671f,93.10241f,93.038155f,92.97433f,92.91093f,92.84794f,92.78536f,92.7232f,92.66143f,92.60007f,92.5391f,92.47852f,92.418335f,92.35853f,92.299095f,92.24004f,92.18135f,92.12303f,92.06508f,92.00748f,91.950226f,91.89333f,91.836784f,91.78058f,91.72471f,91.66918f,91.61398f,91.55911f,91.50457f,91.450356f,91.396454f,91.342865f,91.28959f,91.236626f,91.18397f,91.131615f,91.07956f,91.0278f,90.97633f,90.92516f,90.87428f,90.82368f,90.77336f,90.72333f,90.67357f,90.624084f,90.57487f,90.525925f,90.47725f,90.42883f,90.380684f,90.33279f,90.28515f,90.23777f,90.190636f,90.14375f,90.09712f,90.05073f,90.004585f,89.95867f,89.913f,89.86757f,89.82237f,89.777405f,89.732666f,89.68815f,89.64386f,89.5998f,89.555954f,89.51233f,89.468925f,89.425735f,89.38276f,89.33999f,89.29743f,89.25509f,89.212944f,89.171005f,89.12927f,89.08774f,89.046394f,89.00526f,88.96432f,88.92357f,88.88301f,88.84264f,88.80247f,88.762474f,88.72267f,88.68305f,88.643616f,88.604355f,88.565285f,88.52638f,88.48766f,88.44912f,88.41074f,88.37254f,88.33452f,88.29666f,88.25897f,88.22144f,88.18409f,88.1469f,88.10986f,88.073f,88.036285f,87.99974f,87.96335f,87.92712f,87.89104f,87.85511f,87.81934f,87.78372f,87.74825f,87.71294f,87.677765f,87.642746f,87.607864f,87.573135f,87.53854f,87.504105f,87.4698f,87.43564f,87.40161f,87.36773f,87.333984f,87.30038f,87.2669f,87.23356f,87.200356f,87.16728f,87.13434f,87.10153f,87.06885f,87.0363f,87.003876f,86.97158f,86.93941f,86.90736f,86.875435f,86.84364f,86.811966f,86.78041f,86.74898f,86.71767f,86.68647f,86.655396f,86.624435f,86.5936f,86.562874f,86.53226f,86.50176f,86.47138f,86.44111f,86.41096f,86.380905f,86.35097f,86.321144f,86.29143f,86.26182f,86.232315f,86.20292f,86.17363f,86.14445f,86.11537f,86.086395f,86.057526f,86.028755f,86.000084f,85.97152f,85.943054f,85.91469f,85.88643f,85.85826f,85.83019f,85.802216f,85.774345f,85.74657f,85.71888f,85.69129f,85.6638f,85.6364f,85.60909f,85.58188f,85.554756f,85.527725f,85.500786f,85.47394f,85.44718f,85.42051f,85.39393f,85.36743f,85.34103f,85.31471f,85.288475f,85.26233f,85.23627f,85.2103f,85.1844f,85.15859f,85.13287f,85.10723f,85.08167f,85.05619f,85.0308f,85.005486f,84.98025f,84.95509f,84.93002f,84.90503f,84.88011f,84.85527f,84.83051f,84.805824f,84.78122f,84.75669f,84.73224f,84.707855f,84.683556f,84.659325f,84.63517f,84.611084f,84.58708f,84.56314f,84.53928f,84.51549f,84.491776f,84.468124f,84.44455f,84.42104f,84.397606f,84.37424f,84.350945f,84.32771f,84.30456f,84.28146f,84.258446f,84.23549f,84.2126f,84.18978f,84.16702f,84.14433f,84.12171f,84.09915f,84.07666f,84.05423f,84.03187f,84.00957f,83.98733f,83.96516f,83.94305f,83.921f,83.89901f,83.87708f,83.855225f,83.83342f,83.81168f,83.79f,83.76839f,83.746826f,83.72533f,83.70389f,83.6825f,83.66118f,83.639915f,83.61871f,83.597565f,83.57648f,83.55544f,83.53447f,83.51355f,83.49268f,83.47188f,83.451126f,83.430435f,83.40979f,83.389206f,83.368675f,83.348206f,83.32778f,83.30741f,83.2871f,83.26684f,83.246635f,83.22648f,83.206375f,83.186325f,83.16633f,83.146385f,83.126495f,83.10665f,83.08686f,83.067116f,83.04743f,83.02779f,83.0082f,82.98866f,82.96917f,82.94973f,82.93034f,82.910995f,82.89171f,82.87246f,82.853264f,82.834114f,82.81502f,82.79596f,82.776955f,82.757996f,82.73908f,82.72022f,82.7014f,82.68263f,82.6639f,82.645226f,82.62659f,82.608f,82.589455f,82.57096f,82.552505f,82.534096f,82.51573f,82.497406f,82.47913f,82.4609f,82.44271f,82.42456f,82.40646f,82.388405f,82.370384f,82.35241f,82.33448f,82.31659f,82.298744f,82.28094f,82.263176f,82.24545f,82.22777f,82.21013f,82.19253f,82.17497f,82.157455f,82.13998f,82.12254f,82.10515f,82.08779f,82.07047f,82.05319f,82.03596f,82.01876f,82.0016f,81.98448f,81.9674f,81.950356f,81.93335f,81.91638f,81.89945f,81.88257f,81.865715f,81.8489f,81.83212f,81.81538f,81.798676f,81.78201f,81.76538f,81.74879f,81.73223f,81.71571f,81.69923f,81.68278f,81.66637f,81.649994f,81.63365f,81.61735f,81.60108f,81.58485f,81.56865f,81.55248f,81.536354f,81.520256f,81.504196f,81.488174f,81.472176f,81.45622f,81.4403f,81.42441f,81.408554f,81.39273f,81.37694f,81.36118f,81.34546f,81.32977f,81.31412f,81.29849f,81.2829f,81.26734f,81.25181f,81.23632f,81.220856f,81.20542f,81.190025f,81.17466f,81.159325f,81.14402f,81.128746f,81.1135f,81.0983f,81.083115f,81.06797f,81.05285f,81.037766f,81.022705f,81.00768f,80.99268f,80.97772f,80.96278f,80.94788f,80.93301f,80.91816f,80.90334f,80.88856f,80.873795f,80.85907f,80.84437f,80.829704f,80.81506f,80.800446f,80.785866f,80.77131f,80.75678f,80.74229f,80.72781f,80.71338f,80.69896f,80.68458f,80.67022f,80.65589f,80.64159f,80.62731f,80.61307f,80.59885f,80.584656f,80.570496f,80.55636f,80.542244f,80.52816f,80.51411f,80.50008f,80.48607f,80.47209f,80.458145f,80.444214f,80.43032f,80.41645f,80.4026f,80.38878f,80.374985f,80.36122f,80.34747f,80.33376f,80.32007f,80.306404f,80.29276f,80.279144f,80.26556f,80.25199f,80.23845f,80.22493f,80.21144f,80.197975f,80.18453f,80.17112f,80.15772f,80.144356f,80.13101f,80.11769f,80.1044f,80.091125f,80.07788f,80.06466f,80.05145f,80.03828f,80.02512f,80.012f,79.99889f,79.98581f,79.972755f,79.95972f,79.9467f,79.933716f,79.920746f,79.90781f,79.89488f,79.88199f,79.86911f,79.85626f,79.84343f,79.83062f,79.81783f,79.80508f,79.792336f,79.77962f,79.76692f,79.75424f,79.74159f,79.72896f,79.716354f,79.703766f,79.6912f,79.67866f,79.66614f,79.65363f,79.64115f,79.6287f,79.616264f,79.60384f,79.59145f,79.57908f,79.56673f,79.55439f,79.54208f,79.52979f,79.517525f,79.50527f,79.49305f,79.48084f,79.46865f,79.45649f,79.44434f,79.43221f,79.420105f,79.40802f,79.39596f,79.38391f,79.37189f,79.35988f,79.34789f,79.33593f,79.32398f,79.31205f,79.30015f,79.28825f,79.27639f,79.26454f,79.25271f,79.2409f,79.22911f,79.21734f,79.20558f,79.19385f,79.18214f,79.17043f,79.15876f,79.1471f,79.13546f,79.12384f,79.112236f,79.100655f,79.08909f,79.07754f,79.06601f,79.054504f,79.04301f,79.03153f,79.02008f,79.008644f,78.99722f,78.985825f,78.974434f,78.96307f,78.95172f,78.94039f,78.929085f,78.917786f,78.90651f,78.89525f,78.88401f,78.87279f,78.86158f,78.85039f,78.83922f,78.828064f,78.816925f,78.8058f,78.7947f,78.783615f,78.772545f,78.76149f,78.75046f,78.73944f,78.72844f,78.71745f,78.70648f,78.69553f,78.6846f,78.673676f,78.66278f,78.65189f,78.64102f,78.63017f,78.61933f,78.60851f,78.59771f,78.58692f,78.57616f,78.5654f,78.55466f,78.54394f,78.533226f,78.52254f,78.51186f,78.501205f,78.49056f,78.479935f,78.46932f,78.458725f,78.44814f,78.43758f,78.42703f,78.4165f,78.405975f,78.39547f,78.38499f,78.37451f,78.36405f,78.353615f,78.343185f,78.33277f,78.32238f,78.312f,78.30163f,78.291275f,78.280945f,78.27062f,78.260315f,78.25002f,78.239746f,78.229485f,78.21923f,78.209f,78.198784f,78.188576f,78.17839f,78.16821f,78.15805f,78.1479f,78.13778f,78.127655f,78.11755f,78.10747f,78.09739f,78.08733f,78.077286f,78.06725f,78.057236f,78.04723f,78.03724f,78.02727f,78.0173f,78.007355f,77.99742f,77.9875f,77.97759f,77.967705f,77.957825f,77.94796f,77.9381f,77.92827f,77.91844f,77.90863f,77.898834f,77.889046f,77.87928f,77.86952f,77.85977f,77.850044f,77.840324f,77.83062f,77.82093f,77.81126f,77.80159f,77.79194f,77.782295f,77.772675f,77.76306f,77.75346f,77.74387f,77.7343f,77.72474f,77.715195f,77.70566f,77.69614f,77.68662f,77.67713f,77.66764f,77.65817f,77.64871f,77.63927f,77.62984f,77.620415f,77.611f,77.60161f,77.592224f,77.582855f,77.573494f,77.56415f,77.55481f,77.54549f,77.53618f,77.52688f,77.51759f,77.50832f,77.49906f,77.489815f,77.480576f,77.47135f,77.462135f,77.452934f,77.44375f,77.43457f,77.4254f,77.416245f,77.407104f,77.39798f,77.388855f,77.37975f,77.37066f,77.36157f,77.3525f,77.343445f,77.3344f,77.325356f,77.31633f,77.30732f,77.29832f,77.28932f,77.28034f,77.27138f,77.26242f,77.25348f,77.244545f,77.23562f,77.22671f,77.21781f,77.208916f,77.20004f,77.19118f,77.18232f,77.17348f,77.16464f,77.15582f,77.14701f,77.13821f,77.12942f,77.12064f,77.11187f,77.10312f,77.09437f,77.08563f,77.07691f,77.0682f,77.059494f,77.050804f,77.04212f,77.033455f,77.024796f,77.01614f,77.00751f,76.99888f,76.99026f,76.98165f,76.97306f,76.96447f,76.955894f,76.947334f,76.938774f,76.93023f,76.9217f,76.91318f,76.90466f,76.89616f,76.887665f,76.87918f,76.87071f,76.86225f,76.8538f,76.84536f,76.83693f,76.82851f,76.82009f,76.81169f,76.8033f,76.79492f,76.78655f,76.77819f,76.76984f,76.7615f,76.75317f,76.74484f,76.736534f,76.728226f,76.71994f,76.711655f,76.703384f,76.69512f,76.68687f,76.67863f,76.67039f,76.66216f,76.65395f,76.645744f,76.63755f,76.629364f,76.62119f,76.61302f,76.604866f,76.59672f,76.588585f,76.58045f,76.572334f,76.564224f,76.55612f,76.548035f,76.539955f,76.53188f,76.52382f,76.51576f,76.50772f,76.49969f,76.49166f,76.48364f,76.47563f,76.467636f,76.45965f,76.45167f,76.443695f,76.43573f,76.42778f,76.41984f,76.4119f,76.40398f,76.39606f,76.38815f,76.38025f,76.37236f,76.36448f,76.356606f,76.34874f,76.34089f,76.333046f,76.3252f,76.317375f,76.309555f,76.30175f,76.293945f,76.286156f,76.278366f,76.27059f,76.262825f,76.255066f,76.247314f,76.23958f,76.23184f,76.22412f,76.2164f,76.208694f,76.201f,76.193306f,76.18562f,76.177956f,76.17029f,76.16263f,76.15498f,76.14735f,76.13971f,76.13209f,76.12447f,76.11687f,76.10927f,76.101685f,76.0941f,76.086525f,76.078964f,76.0714f,76.06386f,76.05632f,76.04878f,76.04126f,76.033745f,76.02624f,76.01874f,76.011246f,76.00376f,75.99629f,75.98882f,75.98136f,75.97391f,75.96647f,75.95903f,75.95161f,75.94418f,75.936775f,75.92937f,75.921974f,75.91459f,75.9072f,75.89983f,75.89247f,75.88511f,75.87776f,75.87042f,75.86309f,75.85577f,75.84845f,75.84113f,75.83383f,75.82654f,75.81925f,75.81197f,75.8047f,75.79744f,75.79018f,75.78293f,75.77569f,75.768456f,75.76123f,75.75401f,75.746796f,75.73959f,75.7324f,75.72521f,75.718025f,75.71085f,75.70369f,75.696526f,75.68938f,75.682236f,75.675095f,75.66797f,75.66084f,75.65373f,75.64662f,75.63952f,75.63243f,75.62534f,75.61826f,75.61119f,75.604126f,75.59707f,75.59002f,75.58298f,75.57594f,75.56892f,75.5619f,75.55488f,75.547874f,75.54087f,75.53388f,75.52689f,75.51991f,75.51295f,75.50598f,75.49902f,75.49207f,75.48513f,75.478195f,75.47126f,75.46434f,75.45742f,75.450516f,75.44361f,75.43672f,75.42983f,75.42295f,75.41608f,75.40921f};
class mydspSIG9 {
	
  private:
	
	int fmydspSIG9Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG9() {
		return 0;
	}
	int getNumOutputsmydspSIG9() {
		return 1;
	}
	
	void instanceInitmydspSIG9(int sample_rate) {
		fmydspSIG9Wave0_idx = 0;
	}
	
	void fillmydspSIG9(int count, float* table) {
		for (int i10 = 0; i10 < count; i10 = i10 + 1) {
			table[i10] = fmydspSIG9Wave0[fmydspSIG9Wave0_idx];
			fmydspSIG9Wave0_idx = (1 + fmydspSIG9Wave0_idx) % 2001;
		}
	}

};

static mydspSIG9* newmydspSIG9() { return (mydspSIG9*)new mydspSIG9(); }
static void deletemydspSIG9(mydspSIG9* dsp) { delete dsp; }

const static float fmydspSIG10Wave0[2001] = {137.51273f,137.39922f,137.28569f,137.17213f,137.05855f,136.94493f,136.83128f,136.71762f,136.60391f,136.49019f,136.37643f,136.26266f,136.14885f,136.03502f,135.92116f,135.80727f,135.69336f,135.5794f,135.46544f,135.35144f,135.23743f,135.12337f,135.0093f,134.89519f,134.78105f,134.66689f,134.5527f,134.43849f,134.32425f,134.20998f,134.09567f,133.98135f,133.867f,133.75262f,133.63821f,133.52379f,133.40933f,133.29485f,133.18033f,133.06578f,132.95122f,132.83662f,132.722f,132.60735f,132.49268f,132.37798f,132.26324f,132.14848f,132.0337f,131.91888f,131.80405f,131.68918f,131.5743f,131.45937f,131.34442f,131.22945f,131.11446f,130.99942f,130.88437f,130.76929f,130.65419f,130.53905f,130.42389f,130.3087f,130.19348f,130.07823f,129.96297f,129.84767f,129.73235f,129.617f,129.50162f,129.38622f,129.27078f,129.15533f,129.03984f,128.92433f,128.80879f,128.69324f,128.57764f,128.46202f,128.34637f,128.2307f,128.115f,127.999275f,127.88352f,127.767746f,127.65194f,127.5361f,127.42024f,127.30436f,127.188446f,127.0725f,126.956535f,126.84054f,126.72452f,126.608475f,126.4924f,126.3763f,126.26017f,126.14402f,126.02783f,125.91163f,125.795395f,125.67913f,125.56284f,125.446526f,125.330185f,125.21382f,125.09743f,124.981f,124.864555f,124.74808f,124.63158f,124.51505f,124.39849f,124.28191f,124.165306f,124.04867f,123.93201f,123.81532f,123.6986f,123.58186f,123.465096f,123.3483f,123.231476f,123.11463f,122.99776f,122.88085f,122.76392f,122.64697f,122.52999f,122.41298f,122.295944f,122.17889f,122.0618f,121.94468f,121.82754f,121.71037f,121.59318f,121.47595f,121.35871f,121.24143f,121.12413f,121.006805f,120.88945f,120.77207f,120.65466f,120.53723f,120.41977f,120.302284f,120.18477f,120.06722f,119.94966f,119.83206f,119.71445f,119.5968f,119.479126f,119.36143f,119.2437f,119.125946f,119.00816f,118.89036f,118.77252f,118.65466f,118.53678f,118.41887f,118.30093f,118.18296f,118.06497f,117.94695f,117.8289f,117.71083f,117.592735f,117.47461f,117.35645f,117.23827f,117.12007f,117.00184f,116.88358f,116.7653f,116.64699f,116.52865f,116.41028f,116.29189f,116.17347f,116.05503f,115.936554f,115.81806f,115.69953f,115.580986f,115.46241f,115.3438f,115.225174f,115.106514f,114.98783f,114.86912f,114.75038f,114.63162f,114.512825f,114.39401f,114.27517f,114.156296f,114.0374f,113.91848f,113.79953f,113.68055f,113.56155f,113.44252f,113.32346f,113.20438f,113.085266f,112.96613f,112.84697f,112.727776f,112.60856f,112.48932f,112.37005f,112.250755f,112.13143f,112.01208f,111.8927f,111.7733f,111.65387f,111.53442f,111.41493f,111.295425f,111.17589f,111.05632f,110.93674f,110.817116f,110.69747f,110.577805f,110.45811f,110.33839f,110.218636f,110.09886f,109.97906f,109.85922f,109.73937f,109.619484f,109.49957f,109.37964f,109.259674f,109.13969f,109.01967f,108.89963f,108.77956f,108.659454f,108.53933f,108.41918f,108.299f,108.1788f,108.05857f,107.93831f,107.81802f,107.697716f,107.57738f,107.45701f,107.33662f,107.2162f,107.09576f,106.97528f,106.85478f,106.73426f,106.6137f,106.493126f,106.37252f,106.251884f,106.131226f,106.01054f,105.889824f,105.76908f,105.648315f,105.52752f,105.4067f,105.28585f,105.16498f,105.044075f,104.92314f,104.802185f,104.681206f,104.560196f,104.439156f,104.31809f,104.197f,104.07588f,103.95474f,103.833565f,103.71237f,103.59114f,103.46989f,103.34861f,103.2273f,103.10597f,102.98461f,102.86322f,102.741806f,102.62036f,102.49889f,102.377396f,102.255875f,102.13432f,102.01275f,101.89114f,101.769516f,101.64785f,101.52617f,101.40446f,101.282715f,101.16095f,101.039154f,100.917336f,100.79549f,100.67361f,100.551704f,100.42977f,100.307816f,100.18583f,100.06382f,99.94178f,99.81972f,99.697624f,99.5755f,99.453354f,99.33118f,99.20898f,99.08675f,98.964485f,98.8422f,98.71989f,98.59755f,98.47518f,98.35279f,98.23037f,98.10792f,97.98544f,97.86294f,97.74041f,97.61785f,97.49526f,97.37265f,97.250015f,97.12734f,97.00465f,96.88193f,96.75918f,96.6364f,96.51359f,96.390755f,96.2679f,96.14501f,96.022095f,95.89915f,95.77618f,95.65318f,95.53016f,95.407104f,95.28402f,95.16091f,95.03777f,94.91461f,94.79142f,94.6682f,94.54495f,94.42168f,94.29838f,94.17505f,94.05169f,93.92831f,93.80489f,93.68145f,93.55798f,93.43449f,93.31096f,93.18741f,93.06383f,92.940216f,92.81658f,92.69292f,92.56923f,92.44551f,92.32176f,92.19798f,92.07418f,91.95035f,91.82649f,91.70261f,91.57869f,91.45475f,91.33078f,91.20678f,91.08275f,90.958694f,90.83461f,90.7105f,90.586365f,90.4622f,90.338f,90.213776f,90.08952f,89.96524f,89.840935f,89.7166f,89.59223f,89.46784f,89.34342f,89.21897f,89.0945f,88.969986f,88.84545f,88.720894f,88.5963f,88.47168f,88.34704f,88.22236f,88.09766f,87.97292f,87.84816f,87.72337f,87.59856f,87.47371f,87.34884f,87.22393f,87.099f,86.974045f,86.84905f,86.72404f,86.59899f,86.473915f,86.348816f,86.22368f,86.09852f,85.973335f,85.848114f,85.72287f,85.597595f,85.47229f,85.34696f,85.2216f,85.09621f,84.970795f,84.845345f,84.71987f,84.59437f,84.468834f,84.34327f,84.21768f,84.09206f,83.96641f,83.84073f,83.71503f,83.58929f,83.463524f,83.33773f,83.21191f,83.08606f,82.960175f,82.83427f,82.70833f,82.58236f,82.45637f,82.33034f,82.204285f,82.0782f,81.95209f,81.82594f,81.69977f,81.57357f,81.44734f,81.32108f,81.19479f,81.068474f,80.94212f,80.81575f,80.68935f,80.56291f,80.43645f,80.30995f,80.183426f,80.05688f,79.93029f,79.80368f,79.67704f,79.55037f,79.42367f,79.296936f,79.17018f,79.04339f,78.91657f,78.78973f,78.66284f,78.535934f,78.409004f,78.282036f,78.15504f,78.028015f,77.900955f,77.77387f,77.64675f,77.51961f,77.39243f,77.26523f,77.13799f,77.01073f,76.88344f,76.75611f,76.62875f,76.50137f,76.373955f,76.24651f,76.11904f,75.99154f,75.864f,75.736435f,75.60884f,75.48122f,75.35356f,75.225876f,75.09817f,74.97042f,74.84264f,74.714836f,74.587006f,74.45914f,74.33124f,74.203316f,74.075356f,73.94737f,73.81935f,73.69131f,73.563225f,73.43512f,73.306984f,73.17881f,73.05061f,72.92238f,72.79412f,72.66583f,72.537506f,72.40916f,72.28077f,72.15236f,72.02392f,71.89544f,71.76694f,71.6384f,71.509834f,71.38123f,71.25261f,71.12395f,70.99526f,70.86654f,70.737785f,70.60901f,70.480194f,70.35135f,70.22247f,70.093575f,69.96464f,69.83567f,69.70667f,69.577644f,69.448586f,69.319496f,69.19037f,69.06122f,68.93204f,68.80282f,68.67358f,68.5443f,68.41499f,68.28565f,68.15628f,68.02688f,67.897446f,67.76798f,67.63849f,67.508965f,67.37941f,67.24982f,67.1202f,66.99055f,66.86087f,66.731155f,66.60141f,66.471634f,66.34183f,66.21199f,66.08212f,65.95222f,65.82229f,65.69232f,65.56233f,65.432304f,65.302246f,65.17216f,65.04204f,64.91188f,64.7817f,64.65149f,64.52124f,64.39096f,64.26065f,64.13031f,63.99994f,63.869534f,63.7391f,63.608635f,63.478134f,63.347607f,63.217045f,63.086452f,62.955826f,62.825172f,62.694485f,62.563763f,62.433014f,62.30223f,62.171417f,62.040573f,61.909695f,61.778786f,61.647846f,61.516876f,61.38587f,61.254837f,61.12377f,60.992676f,60.861546f,60.730385f,60.599194f,60.467968f,60.33671f,60.205425f,60.07411f,59.942757f,59.811375f,59.679962f,59.54852f,59.417046f,59.285538f,59.154f,59.02243f,58.890827f,58.759193f,58.62753f,58.495834f,58.36411f,58.23235f,58.100563f,57.968742f,57.83689f,57.70501f,57.573093f,57.44115f,57.309174f,57.177166f,57.045128f,56.91306f,56.78096f,56.64883f,56.516666f,56.384476f,56.25225f,56.12f,55.987713f,55.8554f,55.723053f,55.59068f,55.45827f,55.325836f,55.193367f,55.06087f,54.928345f,54.795784f,54.663197f,54.53058f,54.397934f,54.265255f,54.13255f,53.999813f,53.867046f,53.734253f,53.60143f,53.468575f,53.33569f,53.202778f,53.06984f,52.936867f,52.80387f,52.670845f,52.53779f,52.404705f,52.271595f,52.138454f,52.005287f,51.872093f,51.738873f,51.60562f,51.472343f,51.339043f,51.20571f,51.072353f,50.938972f,50.80556f,50.672127f,50.538666f,50.405178f,50.271667f,50.13813f,50.00457f,49.870983f,49.737373f,49.603737f,49.470078f,49.3364f,49.202694f,49.068966f,48.935215f,48.801445f,48.667652f,48.533836f,48.4f,48.266144f,48.132267f,47.99837f,47.864456f,47.730522f,47.59657f,47.4626f,47.32861f,47.194607f,47.060585f,46.926548f,46.792496f,46.65843f,46.524345f,46.39025f,46.256145f,46.122025f,45.987896f,45.853752f,45.7196f,45.58544f,45.45127f,45.317097f,45.182915f,45.048725f,44.91453f,44.780334f,44.646133f,44.51193f,44.377728f,44.243523f,44.10932f,43.97512f,43.840927f,43.706738f,43.57255f,43.438374f,43.304207f,43.170048f,43.035904f,42.90177f,42.767654f,42.633553f,42.49947f,42.365406f,42.231365f,42.097347f,41.963356f,41.82939f,41.695457f,41.561554f,41.427685f,41.293854f,41.16006f,41.026306f,40.892597f,40.758934f,40.62532f,40.491756f,40.358246f,40.224796f,40.091404f,39.958076f,39.824818f,39.691628f,39.55851f,39.42547f,39.29251f,39.159637f,39.02685f,38.894157f,38.761562f,38.629066f,38.496677f,38.3644f,38.23223f,38.10019f,37.96827f,37.836475f,37.704823f,37.573307f,37.441936f,37.31072f,37.17966f,37.048763f,36.918037f,36.78749f,36.657124f,36.526947f,36.396965f,36.26719f,36.137623f,36.008278f,35.879154f,35.750267f,35.621624f,35.493225f,35.36509f,35.237217f,35.109623f,34.98231f,34.855293f,34.728577f,34.602173f,34.47609f,34.350334f,34.224926f,34.099865f,33.975163f,33.850834f,33.726887f,33.603333f,33.48018f,33.357437f,33.235123f,33.113243f,32.991806f,32.87083f,32.75032f,32.63029f,32.510754f,32.391716f,32.273193f,32.155193f,32.03773f,31.920813f,31.804455f,31.688665f,31.573456f,31.458838f,31.344822f,31.231417f,31.118637f,31.006489f,30.894985f,30.784136f,30.67395f,30.564438f,30.455606f,30.34747f,30.24003f,30.133303f,30.02729f,29.922007f,29.817453f,29.713642f,29.610577f,29.508268f,29.40672f,29.305937f,29.205925f,29.106693f,29.008242f,28.910578f,28.813705f,28.717628f,28.622347f,28.527868f,28.43419f,28.34132f,28.249256f,28.158f,28.067554f,27.977919f,27.889093f,27.801077f,27.713871f,27.627474f,27.541883f,27.457098f,27.373117f,27.289938f,27.20756f,27.125975f,27.045185f,26.965183f,26.88597f,26.807535f,26.729881f,26.653f,26.576887f,26.501537f,26.426947f,26.353111f,26.280022f,26.207676f,26.136068f,26.06519f,25.995035f,25.9256f,25.856876f,25.78886f,25.721542f,25.654917f,25.588976f,25.523716f,25.459127f,25.395205f,25.331942f,25.26933f,25.20736f,25.146029f,25.08533f,25.025251f,24.965792f,24.90694f,24.84869f,24.791037f,24.733969f,24.677485f,24.621572f,24.566229f,24.511444f,24.457212f,24.403528f,24.350384f,24.297771f,24.245684f,24.194117f,24.143063f,24.092514f,24.042465f,23.99291f,23.943842f,23.895254f,23.84714f,23.799494f,23.75231f,23.705582f,23.659302f,23.613468f,23.568071f,23.523106f,23.478567f,23.43445f,23.390749f,23.347456f,23.304567f,23.26208f,23.219982f,23.178276f,23.136951f,23.096006f,23.055433f,23.015228f,22.975388f,22.935904f,22.896774f,22.857996f,22.81956f,22.781464f,22.743704f,22.706274f,22.66917f,22.632391f,22.595928f,22.55978f,22.523941f,22.488409f,22.453178f,22.418245f,22.383608f,22.349258f,22.315197f,22.281418f,22.24792f,22.214695f,22.181744f,22.14906f,22.116642f,22.084486f,22.05259f,22.020947f,21.989557f,21.958416f,21.92752f,21.89687f,21.866457f,21.83628f,21.80634f,21.776628f,21.747147f,21.717888f,21.688854f,21.66004f,21.631441f,21.60306f,21.574888f,21.546926f,21.519173f,21.491623f,21.464275f,21.437126f,21.410175f,21.38342f,21.356857f,21.330484f,21.3043f,21.278301f,21.252487f,21.226854f,21.2014f,21.176126f,21.151026f,21.1261f,21.101345f,21.07676f,21.052343f,21.028091f,21.004004f,20.980078f,20.956314f,20.932707f,20.909258f,20.885963f,20.862822f,20.839832f,20.816994f,20.794304f,20.771759f,20.749361f,20.727106f,20.704994f,20.683022f,20.661188f,20.639494f,20.617935f,20.59651f,20.57522f,20.554062f,20.533033f,20.512135f,20.491364f,20.47072f,20.450201f,20.429806f,20.409534f,20.389383f,20.369352f,20.349442f,20.329649f,20.309973f,20.290411f,20.270966f,20.251633f,20.23241f,20.2133f,20.194302f,20.175411f,20.156628f,20.13795f,20.11938f,20.100914f,20.082552f,20.064293f,20.046135f,20.028078f,20.010122f,19.992264f,19.974503f,19.95684f,19.939274f,19.921803f,19.904425f,19.887142f,19.86995f,19.852852f,19.835842f,19.818924f,19.802095f,19.785357f,19.768703f,19.752138f,19.735659f,19.719263f,19.702955f,19.68673f,19.670586f,19.654526f,19.638548f,19.62265f,19.606833f,19.591095f,19.575436f,19.559856f,19.544352f,19.528927f,19.513577f,19.4983f,19.4831f,19.467976f,19.452925f,19.437946f,19.42304f,19.408205f,19.393442f,19.378748f,19.364126f,19.349573f,19.335089f,19.320673f,19.306324f,19.292044f,19.277828f,19.26368f,19.249598f,19.235579f,19.221626f,19.207737f,19.19391f,19.18015f,19.166449f,19.152811f,19.139235f,19.12572f,19.112263f,19.09887f,19.085533f,19.072258f,19.05904f,19.045881f,19.03278f,19.019735f,19.006748f,18.993818f,18.980944f,18.968124f,18.95536f,18.942652f,18.929998f,18.917397f,18.90485f,18.892357f,18.879915f,18.867527f,18.855192f,18.842907f,18.830673f,18.818491f,18.806358f,18.794277f,18.782244f,18.770262f,18.758327f,18.746443f,18.734608f,18.722818f,18.711079f,18.699385f,18.687738f,18.676138f,18.664585f,18.653078f,18.641617f,18.6302f,18.61883f,18.607502f,18.59622f,18.584984f,18.57379f,18.56264f,18.55153f,18.540468f,18.529446f,18.518467f,18.50753f,18.496635f,18.48578f,18.47497f,18.464197f,18.453466f,18.442778f,18.432127f,18.421516f,18.410946f,18.400415f,18.389923f,18.379469f,18.369055f,18.358679f,18.348341f,18.338041f,18.327778f,18.317553f,18.307364f,18.297213f,18.287098f,18.277021f,18.26698f,18.256973f,18.247002f,18.237068f,18.227169f,18.217304f,18.207474f,18.19768f,18.18792f,18.178194f,18.1685f,18.158842f,18.149218f,18.139626f,18.130068f,18.120543f,18.11105f,18.101591f,18.092163f,18.082767f,18.073404f,18.064072f,18.054771f,18.045504f,18.036266f,18.02706f,18.017885f,18.00874f,17.999626f,17.990541f,17.981487f,17.972464f,17.963469f,17.954504f,17.94557f,17.936665f,17.927788f,17.91894f,17.91012f,17.901329f,17.892567f,17.883833f,17.875128f,17.86645f,17.8578f,17.849178f,17.840584f,17.832016f,17.823475f,17.814962f,17.806475f,17.798016f,17.789583f,17.781176f,17.772795f,17.76444f,17.756113f,17.74781f,17.739532f,17.731281f,17.723055f,17.714855f,17.706678f,17.698528f,17.690403f,17.682302f,17.674227f,17.666174f,17.658146f,17.650143f,17.642164f,17.634209f,17.626278f,17.61837f,17.610485f,17.602625f,17.594786f,17.586973f,17.579182f,17.571413f,17.563667f,17.555944f,17.548244f,17.540567f,17.532911f,17.525278f,17.517668f,17.510078f,17.502512f,17.494967f,17.487442f,17.47994f,17.47246f,17.465002f,17.457563f,17.450146f,17.44275f,17.435375f,17.428022f,17.420689f,17.413376f,17.406084f,17.398811f,17.39156f,17.384329f,17.377117f,17.369926f,17.362755f,17.355604f,17.348473f,17.34136f,17.334267f,17.327194f,17.32014f,17.313107f,17.30609f,17.299093f,17.292116f,17.285158f,17.278217f,17.271296f,17.264395f,17.25751f,17.250645f,17.243797f,17.236967f,17.230156f,17.223364f,17.216589f,17.209831f,17.203093f,17.196371f,17.189667f,17.182981f,17.176311f,17.16966f,17.163027f,17.15641f,17.14981f,17.143229f,17.136662f,17.130114f,17.12358f,17.117067f,17.110569f,17.104086f,17.097622f,17.091173f,17.08474f,17.078325f,17.071924f,17.06554f,17.059174f,17.052822f,17.046486f,17.040167f,17.033863f,17.027575f,17.021303f,17.015047f,17.008804f,17.002579f,16.996368f,16.990173f,16.983994f,16.977829f,16.97168f,16.965546f,16.959425f,16.953321f,16.947231f,16.941156f,16.935097f,16.92905f,16.92302f,16.917004f,16.911003f,16.905014f,16.899042f,16.893084f,16.887138f,16.881208f,16.875292f,16.86939f,16.863503f,16.857628f,16.851767f,16.84592f,16.840088f,16.834269f,16.828463f,16.822672f,16.816893f,16.811129f,16.805378f,16.799639f,16.793915f,16.788202f,16.782505f,16.77682f,16.771147f,16.765488f,16.759842f,16.75421f,16.748589f,16.742981f,16.737387f,16.731806f,16.726236f,16.72068f,16.715136f,16.709604f,16.704084f,16.698578f,16.693085f,16.687601f,16.682133f,16.676674f,16.67123f,16.665796f,16.660376f,16.654966f,16.649569f,16.644184f,16.638811f,16.63345f,16.6281f,16.62276f,16.617435f,16.61212f,16.606817f,16.601526f,16.596247f,16.590979f,16.58572f,16.580475f,16.575241f,16.570017f,16.564806f,16.559605f,16.554417f,16.549238f,16.544071f,16.538914f,16.53377f,16.528635f,16.523512f,16.5184f,16.513298f,16.508207f,16.503126f,16.498058f,16.492998f,16.487951f,16.482914f,16.477886f,16.47287f,16.467863f,16.462868f,16.457884f,16.452908f,16.447945f,16.44299f,16.438046f,16.433111f,16.428188f,16.423275f,16.418371f,16.413479f,16.408594f,16.40372f,16.398857f,16.394005f,16.38916f,16.384327f,16.379503f,16.37469f,16.369884f,16.36509f,16.360304f,16.355528f,16.350761f,16.346006f,16.341259f,16.336521f,16.331793f,16.327074f,16.322365f,16.317665f,16.312975f,16.308294f,16.303621f,16.298958f,16.294306f,16.289661f,16.285025f,16.2804f,16.275782f,16.271173f,16.266575f,16.261984f,16.257402f,16.25283f,16.248266f,16.243711f,16.239166f,16.234629f,16.2301f,16.22558f,16.22107f,16.216568f,16.212074f,16.207588f,16.203112f,16.198645f,16.194185f,16.189734f,16.185291f,16.180857f,16.176432f,16.172014f,16.167604f,16.163204f,16.158812f,16.154427f,16.150051f,16.145683f,16.141323f,16.136972f,16.13263f,16.128294f,16.123966f,16.119648f,16.115335f,16.111032f,16.106737f,16.10245f,16.098171f,16.093899f,16.089636f,16.085379f,16.081131f,16.07689f,16.072659f,16.068434f,16.064217f,16.060007f,16.055805f,16.05161f,16.047424f,16.043245f,16.039074f,16.03491f,16.030754f,16.026604f,16.022463f,16.018328f,16.014202f,16.010082f,16.00597f,16.001865f,15.997768f,15.993678f,15.989595f,15.98552f,15.981452f,15.97739f,15.973336f,15.96929f,15.96525f,15.961218f,15.957192f,15.953174f,15.9491625f,15.945158f,15.94116f,15.93717f,15.933187f,15.92921f,15.92524f,15.921277f,15.917321f,15.913372f,15.90943f,15.905495f,15.901566f,15.897644f,15.893728f,15.88982f,15.885919f,15.882023f,15.878135f,15.874252f,15.870378f,15.8665085f,15.862647f,15.858791f,15.854942f,15.8511f,15.847264f,15.843434f,15.839611f,15.835794f,15.8319845f,15.82818f,15.824384f,15.820592f,15.816808f,15.813029f,15.809257f,15.805491f,15.801731f,15.797978f,15.794231f,15.79049f,15.786756f,15.783027f,15.7793045f,15.775588f,15.771878f,15.768174f,15.764476f,15.760784f,15.757098f,15.753419f,15.749744f,15.746077f,15.742415f,15.738759f,15.735109f,15.731465f,15.727827f,15.724195f,15.720569f,15.716948f,15.713333f,15.709724f,15.706121f,15.702524f,15.698933f,15.695347f,15.691767f,15.688192f,15.684624f,15.681061f,15.677504f,15.673952f,15.670406f,15.666865f,15.663331f,15.6598015f,15.656279f,15.6527605f,15.649248f,15.6457405f,15.64224f,15.638743f,15.635253f,15.631768f,15.628288f,15.624814f,15.6213455f,15.617883f,15.614425f,15.610972f,15.607525f,15.604083f,15.600647f,15.597216f,15.59379f,15.590369f,15.586954f,15.583544f,15.580139f,15.576739f,15.573345f,15.569956f,15.566571f,15.563193f,15.559819f,15.556451f,15.553087f,15.549729f,15.546376f,15.543028f,15.539684f,15.536346f,15.533013f,15.529686f,15.526363f,15.523046f,15.519732f,15.516424f,15.513122f,15.509824f,15.506531f,15.5032425f,15.499959f,15.49668f,15.493407f,15.490139f,15.486875f,15.483616f,15.480362f,15.477113f,15.473868f,15.470629f,15.467394f,15.464164f,15.460938f,15.457718f,15.454502f,15.451291f,15.448085f,15.444882f,15.441686f,15.438493f,15.435306f,15.432122f,15.428944f,15.42577f,15.422601f,15.419436f,15.416276f,15.413121f,15.40997f,15.406823f,15.403682f,15.400544f,15.397411f,15.394283f,15.391159f,15.388041f,15.384925f,15.381815f,15.378709f,15.3756075f,15.37251f,15.369417f,15.366329f,15.363245f,15.360166f,15.357091f,15.35402f,15.350953f,15.347891f,15.344833f,15.34178f,15.338731f,15.335686f,15.332645f,15.329609f,15.326576f,15.323549f,15.320525f,15.317506f,15.31449f,15.31148f,15.308473f,15.30547f,15.302472f,15.299478f,15.296487f,15.293501f,15.29052f,15.287541f,15.284568f,15.281598f,15.278633f,15.275672f,15.272715f,15.269761f,15.266811f,15.263866f,15.260925f,15.257988f,15.255055f,15.252126f,15.249201f,15.24628f,15.243362f,15.24045f,15.23754f,15.234635f,15.231733f,15.228836f,15.225943f,15.223053f,15.220167f,15.217285f,15.214407f,15.211534f,15.208663f,15.205796f,15.202934f,15.200075f,15.19722f,15.194369f,15.191522f,15.188678f,15.185839f,15.183002f,15.18017f,15.177341f,15.174517f,15.171696f,15.168879f,15.166065f,15.163256f,15.160449f,15.157647f,15.154848f,15.152053f,15.149261f,15.146474f,15.14369f,15.140909f,15.138133f,15.13536f,15.13259f,15.129824f,15.127062f,15.124303f,15.121548f,15.118796f,15.116048f,15.113303f,15.110562f,15.107825f,15.105091f,15.102361f,15.099634f,15.09691f,15.094192f,15.091475f,15.088762f,15.086053f,15.083346f,15.080645f,15.077946f,15.07525f,15.072557f,15.069869f,15.0671835f,15.064502f,15.061824f,15.059149f,15.056477f,15.053808f,15.051144f,15.048482f,15.045824f,15.043169f,15.040518f,15.037869f,15.035225f,15.032583f,15.029945f,15.02731f,15.024678f,15.022051f,15.019425f,15.016804f,15.014185f,15.01157f,15.008958f,15.00635f,15.003744f,15.001142f,14.998543f,14.995947f,14.993355f,14.990766f,14.988179f,14.985596f,14.983016f,14.98044f,14.977866f,14.975296f,14.972729f,14.970165f,14.967605f,14.965047f,14.962492f,14.959941f,14.957392f,14.954847f,14.952305f,14.949766f,14.94723f,14.944697f,14.942167f,14.939641f,14.937117f,14.934596f,14.932078f,14.929564f,14.9270525f,14.924544f,14.922039f,14.919537f,14.917037f,14.91454f,14.912047f,14.909556f,14.907069f,14.904585f,14.902103f,14.899625f,14.897149f,14.894676f,14.892207f,14.88974f,14.887276f,14.884815f,14.882358f,14.879902f,14.87745f,14.875001f,14.872554f,14.8701105f,14.86767f,14.865232f,14.862798f,14.860366f,14.857936f,14.85551f,14.853086f,14.850666f,14.8482485f,14.845833f,14.843421f,14.841012f,14.838605f,14.836202f,14.8338f,14.831403f,14.829007f,14.826614f,14.824224f,14.821837f,14.819453f,14.817072f,14.814693f,14.812317f,14.809944f,14.807573f,14.805205f,14.80284f,14.800478f,14.798119f,14.795762f,14.793407f,14.791056f,14.788707f,14.786361f,14.784018f,14.781676f,14.779338f,14.777002f,14.77467f,14.77234f,14.770012f,14.767687f,14.765365f,14.763045f,14.760728f,14.758414f,14.756103f,14.753793f,14.751487f,14.749183f,14.7468815f,14.744582f,14.742286f,14.739992f,14.737701f,14.735413f,14.733127f,14.730844f,14.728562f,14.726284f,14.724009f,14.721735f,14.719464f,14.717196f,14.714931f,14.712667f,14.710407f,14.708149f};
class mydspSIG10 {
	
  private:
	
	int fmydspSIG10Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG10() {
		return 0;
	}
	int getNumOutputsmydspSIG10() {
		return 1;
	}
	
	void instanceInitmydspSIG10(int sample_rate) {
		fmydspSIG10Wave0_idx = 0;
	}
	
	void fillmydspSIG10(int count, float* table) {
		for (int i11 = 0; i11 < count; i11 = i11 + 1) {
			table[i11] = fmydspSIG10Wave0[fmydspSIG10Wave0_idx];
			fmydspSIG10Wave0_idx = (1 + fmydspSIG10Wave0_idx) % 2001;
		}
	}

};

static mydspSIG10* newmydspSIG10() { return (mydspSIG10*)new mydspSIG10(); }
static void deletemydspSIG10(mydspSIG10* dsp) { delete dsp; }

const static float fmydspSIG11Wave0[2001] = {214.80658f,214.65059f,214.49445f,214.33815f,214.1817f,214.0251f,213.86835f,213.71146f,213.5544f,213.39722f,213.23987f,213.08238f,212.92476f,212.76698f,212.60907f,212.451f,212.2928f,212.13446f,211.97597f,211.81734f,211.65857f,211.49966f,211.34062f,211.18144f,211.02213f,210.86267f,210.70308f,210.54335f,210.38348f,210.2235f,210.06335f,209.90309f,209.7427f,209.58217f,209.42152f,209.26073f,209.09981f,208.93877f,208.77759f,208.61629f,208.45485f,208.29329f,208.1316f,207.96979f,207.80786f,207.6458f,207.48361f,207.32129f,207.15886f,206.9963f,206.83363f,206.67082f,206.5079f,206.34486f,206.1817f,206.01842f,205.85501f,205.6915f,205.52786f,205.3641f,205.20023f,205.03624f,204.87213f,204.70792f,204.54358f,204.37912f,204.21455f,204.04988f,203.88509f,203.72018f,203.55516f,203.39003f,203.2248f,203.05943f,202.89398f,202.72841f,202.56271f,202.39693f,202.23102f,202.06502f,201.8989f,201.73267f,201.56635f,201.3999f,201.23335f,201.0667f,200.89995f,200.73308f,200.56612f,200.39905f,200.23187f,200.06459f,199.8972f,199.72972f,199.56213f,199.39444f,199.22665f,199.05876f,198.89078f,198.72267f,198.55449f,198.3862f,198.2178f,198.04932f,197.88072f,197.71204f,197.54324f,197.37436f,197.20538f,197.03632f,196.86714f,196.69786f,196.5285f,196.35904f,196.18948f,196.01984f,195.8501f,195.68025f,195.51033f,195.3403f,195.17018f,194.99997f,194.82967f,194.65927f,194.48878f,194.3182f,194.14754f,193.97678f,193.80592f,193.63498f,193.46396f,193.29283f,193.12163f,192.95033f,192.77895f,192.60747f,192.4359f,192.26425f,192.09251f,191.92068f,191.74878f,191.57678f,191.4047f,191.23253f,191.06027f,190.88792f,190.7155f,190.54298f,190.37039f,190.19771f,190.02495f,189.8521f,189.67917f,189.50615f,189.33305f,189.15987f,188.9866f,188.81326f,188.63983f,188.46632f,188.29274f,188.11906f,187.94531f,187.77148f,187.59758f,187.42358f,187.24951f,187.07536f,186.90112f,186.72682f,186.55243f,186.37796f,186.20341f,186.0288f,185.8541f,185.6793f,185.50446f,185.32953f,185.15451f,184.97943f,184.80426f,184.62903f,184.4537f,184.27832f,184.10284f,183.9273f,183.7517f,183.576f,183.40024f,183.2244f,183.04848f,182.87248f,182.69643f,182.5203f,182.34407f,182.1678f,181.99144f,181.81502f,181.6385f,181.46194f,181.2853f,181.10858f,180.9318f,180.75493f,180.578f,180.401f,180.22392f,180.04678f,179.86957f,179.69228f,179.51492f,179.33751f,179.16f,178.98245f,178.80481f,178.6271f,178.44934f,178.2715f,178.0936f,177.91562f,177.73758f,177.55946f,177.38129f,177.20303f,177.02472f,176.84634f,176.6679f,176.48938f,176.31079f,176.13214f,175.95343f,175.77466f,175.59581f,175.41689f,175.23792f,175.05887f,174.87976f,174.70059f,174.52135f,174.34204f,174.16267f,173.98325f,173.80374f,173.62419f,173.44456f,173.26488f,173.08513f,172.9053f,172.72543f,172.54549f,172.36548f,172.18541f,172.00528f,171.82509f,171.64484f,171.46452f,171.28415f,171.1037f,170.9232f,170.74263f,170.56201f,170.38133f,170.20058f,170.01978f,169.8389f,169.65797f,169.47697f,169.29593f,169.11482f,168.93365f,168.75243f,168.57114f,168.38979f,168.20837f,168.0269f,167.84537f,167.66379f,167.48215f,167.30045f,167.11868f,166.93686f,166.75497f,166.57304f,166.39104f,166.20898f,166.02689f,165.84471f,165.66249f,165.4802f,165.29787f,165.11546f,164.93301f,164.75049f,164.56793f,164.3853f,164.20262f,164.01988f,163.8371f,163.65424f,163.47133f,163.28838f,163.10536f,162.92229f,162.73915f,162.55597f,162.37274f,162.18944f,162.00609f,161.8227f,161.63924f,161.45572f,161.27216f,161.08853f,160.90486f,160.72113f,160.53735f,160.35352f,160.16962f,159.98567f,159.80168f,159.61763f,159.43353f,159.24937f,159.06516f,158.88089f,158.69658f,158.5122f,158.32779f,158.14333f,157.9588f,157.77422f,157.58958f,157.4049f,157.22017f,157.03539f,156.85056f,156.66566f,156.48073f,156.29573f,156.11069f,155.9256f,155.74045f,155.55525f,155.37001f,155.18472f,154.99937f,154.81398f,154.62852f,154.44302f,154.25748f,154.07188f,153.88623f,153.70053f,153.51479f,153.329f,153.14314f,152.95724f,152.7713f,152.58531f,152.39928f,152.21318f,152.02704f,151.84085f,151.6546f,151.46832f,151.28198f,151.0956f,150.90916f,150.72269f,150.53616f,150.34958f,150.16296f,149.97629f,149.78957f,149.6028f,149.41599f,149.22913f,149.0422f,148.85526f,148.66826f,148.4812f,148.2941f,148.10696f,147.91977f,147.73253f,147.54524f,147.35791f,147.17053f,146.98311f,146.79564f,146.60812f,146.42056f,146.23296f,146.0453f,145.8576f,145.66985f,145.48206f,145.29422f,145.10634f,144.91841f,144.73044f,144.54242f,144.35437f,144.16626f,143.9781f,143.7899f,143.60167f,143.41338f,143.22505f,143.03667f,142.84825f,142.65979f,142.47128f,142.28273f,142.09413f,141.90549f,141.71681f,141.52808f,141.33931f,141.1505f,140.96164f,140.77274f,140.58379f,140.3948f,140.20576f,140.0167f,139.82758f,139.63841f,139.44922f,139.25996f,139.07068f,138.88135f,138.69197f,138.50255f,138.3131f,138.1236f,137.93405f,137.74446f,137.55484f,137.36517f,137.17546f,136.9857f,136.79591f,136.60608f,136.4162f,136.22627f,136.03632f,135.84631f,135.65628f,135.46619f,135.27606f,135.0859f,134.89569f,134.70544f,134.51515f,134.32483f,134.13446f,133.94405f,133.7536f,133.56311f,133.37257f,133.182f,132.9914f,132.80074f,132.61005f,132.41931f,132.22855f,132.03773f,131.84688f,131.65599f,131.46506f,131.2741f,131.08308f,130.89203f,130.70094f,130.50981f,130.31865f,130.12744f,129.9362f,129.74492f,129.5536f,129.36224f,129.17084f,128.9794f,128.78793f,128.59642f,128.40486f,128.21327f,128.02165f,127.82998f,127.638275f,127.44653f,127.25475f,127.062935f,126.87108f,126.679184f,126.48725f,126.29528f,126.10328f,125.91123f,125.719154f,125.52703f,125.33488f,125.142685f,124.950455f,124.75819f,124.56589f,124.37355f,124.18117f,123.988754f,123.79631f,123.60382f,123.4113f,123.21874f,123.026146f,122.83351f,122.64085f,122.44814f,122.2554f,122.06263f,121.86982f,121.67697f,121.484085f,121.29117f,121.09821f,120.90523f,120.712204f,120.51914f,120.32605f,120.13292f,119.93975f,119.74655f,119.553314f,119.36005f,119.16674f,118.973404f,118.78003f,118.58662f,118.39318f,118.1997f,118.00619f,117.812645f,117.619064f,117.425446f,117.2318f,117.03812f,116.8444f,116.65065f,116.45686f,116.26305f,116.0692f,115.87531f,115.6814f,115.48744f,115.29346f,115.09944f,114.90539f,114.7113f,114.51719f,114.32304f,114.12885f,113.93464f,113.740395f,113.54611f,113.3518f,113.15745f,112.96307f,112.76866f,112.57422f,112.379745f,112.18524f,111.9907f,111.79613f,111.601524f,111.40689f,111.21223f,111.017525f,110.8228f,110.62804f,110.43325f,110.23843f,110.04357f,109.84869f,109.65377f,109.45882f,109.26384f,109.06883f,108.873795f,108.67873f,108.48362f,108.28849f,108.09333f,107.89814f,107.70291f,107.50766f,107.31238f,107.117065f,106.92172f,106.72636f,106.53095f,106.33552f,106.14006f,105.94457f,105.749054f,105.553505f,105.357925f,105.162315f,104.96668f,104.77102f,104.575325f,104.3796f,104.18385f,103.98807f,103.79226f,103.59643f,103.40056f,103.204666f,103.00875f,102.8128f,102.61682f,102.420815f,102.224785f,102.028725f,101.83264f,101.63653f,101.440384f,101.24422f,101.04802f,100.85179f,100.65555f,100.45927f,100.26296f,100.066635f,99.87028f,99.6739f,99.477486f,99.28105f,99.084595f,98.88811f,98.69159f,98.495056f,98.29849f,98.1019f,97.90529f,97.70865f,97.51198f,97.31529f,97.118576f,96.92184f,96.725075f,96.52828f,96.331474f,96.134636f,95.937775f,95.74089f,95.543976f,95.347046f,95.15009f,94.95311f,94.7561f,94.55908f,94.36203f,94.16496f,93.967865f,93.77075f,93.57361f,93.37645f,93.17927f,92.98206f,92.784836f,92.587585f,92.39031f,92.19302f,91.99571f,91.79838f,91.60102f,91.40365f,91.20625f,91.00884f,90.81141f,90.61395f,90.41648f,90.21899f,90.02148f,89.823944f,89.626396f,89.428825f,89.23124f,89.03364f,88.83601f,88.638374f,88.44072f,88.24305f,88.04536f,87.84765f,87.649925f,87.452194f,87.25444f,87.05667f,86.85889f,86.66109f,86.46327f,86.26544f,86.067604f,85.86975f,85.67188f,85.47401f,85.276115f,85.07821f,84.880295f,84.682365f,84.48443f,84.28648f,84.088524f,83.89056f,83.69258f,83.4946f,83.29661f,83.09861f,82.900604f,82.70259f,82.50457f,82.30655f,82.10852f,81.910484f,81.71245f,81.51441f,81.31637f,81.118324f,80.92028f,80.72224f,80.52419f,80.32616f,80.12811f,79.93008f,79.73204f,79.53401f,79.33599f,79.13797f,78.939964f,78.74196f,78.54397f,78.345985f,78.14802f,77.95006f,77.75211f,77.554184f,77.35627f,77.15838f,76.9605f,76.76264f,76.564804f,76.36699f,76.169205f,75.971436f,75.773705f,75.576f,75.37832f,75.18068f,74.98307f,74.78549f,74.58796f,74.390465f,74.19301f,73.9956f,73.79824f,73.60092f,73.40366f,73.20645f,73.0093f,72.81221f,72.61517f,72.418205f,72.221306f,72.024475f,71.82772f,71.631035f,71.43443f,71.237915f,71.04149f,70.845146f,70.648895f,70.45275f,70.2567f,70.06076f,69.86493f,69.66921f,69.47362f,69.278145f,69.0828f,68.88759f,68.69252f,68.4976f,68.30282f,68.1082f,67.91374f,67.71945f,67.52534f,67.3314f,67.13765f,66.94409f,66.75074f,66.557594f,66.36466f,66.17196f,65.97948f,65.78725f,65.59525f,65.40352f,65.21205f,65.02086f,64.82995f,64.63933f,64.44901f,64.25901f,64.069336f,63.87999f,63.690987f,63.50234f,63.31406f,63.12616f,62.938644f,62.751537f,62.56484f,62.37857f,62.192738f,62.007362f,61.82245f,61.638016f,61.454075f,61.27064f,61.087727f,60.90535f,60.72352f,60.542255f,60.36157f,60.181477f,60.001995f,59.82314f,59.64492f,59.467358f,59.290466f,59.11426f,58.93876f,58.763973f,58.589924f,58.416626f,58.24409f,58.07234f,57.901382f,57.73124f,57.561928f,57.393456f,57.225845f,57.05911f,56.89326f,56.728317f,56.564293f,56.401203f,56.239056f,56.07787f,55.91766f,55.758434f,55.60021f,55.442997f,55.286804f,55.13165f,54.977543f,54.82449f,54.672504f,54.521595f,54.37177f,54.223038f,54.075413f,53.928894f,53.783493f,53.639214f,53.496067f,53.354053f,53.21318f,53.07345f,52.934868f,52.797436f,52.66116f,52.52604f,52.392075f,52.25927f,52.12762f,51.997135f,51.867805f,51.739635f,51.612617f,51.486755f,51.362045f,51.238483f,51.116066f,50.99479f,50.874653f,50.755646f,50.637768f,50.52101f,50.40537f,50.29084f,50.17741f,50.065083f,49.953842f,49.843685f,49.734604f,49.626587f,49.519634f,49.413727f,49.308865f,49.205036f,49.102234f,49.000446f,48.89967f,48.799885f,48.70109f,48.603275f,48.50643f,48.410545f,48.31561f,48.22161f,48.128544f,48.0364f,47.945164f,47.854828f,47.76538f,47.676815f,47.58912f,47.502285f,47.4163f,47.331158f,47.246845f,47.163353f,47.080673f,46.998795f,46.917706f,46.8374f,46.757862f,46.679092f,46.60107f,46.523796f,46.447258f,46.37144f,46.29634f,46.221947f,46.148254f,46.07525f,46.002922f,45.931267f,45.860275f,45.78994f,45.720245f,45.65119f,45.582767f,45.51496f,45.447773f,45.381184f,45.315197f,45.249798f,45.18498f,45.120735f,45.057056f,44.99394f,44.93137f,44.86935f,44.807865f,44.74691f,44.68648f,44.626568f,44.567165f,44.508266f,44.449867f,44.391956f,44.33453f,44.27758f,44.221104f,44.165092f,44.109543f,44.054447f,43.999798f,43.945595f,43.891827f,43.83849f,43.785576f,43.733086f,43.68101f,43.629345f,43.578087f,43.527225f,43.47676f,43.426685f,43.376995f,43.327682f,43.278748f,43.230186f,43.181988f,43.13415f,43.086674f,43.03955f,42.992775f,42.946342f,42.900253f,42.8545f,42.80908f,42.763985f,42.719215f,42.674767f,42.63064f,42.58682f,42.54331f,42.500107f,42.457207f,42.414604f,42.372295f,42.33028f,42.288555f,42.247112f,42.20595f,42.165073f,42.124466f,42.084133f,42.04407f,42.004272f,41.964737f,41.92546f,41.886444f,41.847683f,41.809174f,41.770912f,41.732895f,41.69512f,41.657593f,41.620296f,41.58324f,41.546417f,41.509823f,41.473457f,41.437317f,41.401398f,41.365704f,41.330227f,41.294964f,41.25992f,41.225082f,41.190456f,41.156036f,41.121822f,41.087814f,41.054005f,41.020393f,40.98698f,40.953762f,40.920734f,40.8879f,40.855255f,40.822796f,40.790524f,40.758434f,40.726524f,40.694798f,40.663246f,40.631874f,40.600674f,40.56965f,40.53879f,40.508106f,40.47759f,40.44724f,40.417053f,40.387028f,40.357166f,40.327465f,40.29792f,40.268536f,40.239304f,40.210228f,40.181305f,40.15253f,40.12391f,40.095436f,40.06711f,40.03893f,40.01089f,39.982998f,39.955246f,39.927635f,39.900166f,39.87283f,39.845634f,39.818573f,39.791645f,39.764854f,39.73819f,39.711662f,39.68526f,39.65899f,39.632843f,39.606827f,39.580933f,39.555164f,39.52952f,39.503998f,39.478596f,39.453312f,39.42815f,39.403107f,39.378178f,39.353367f,39.32867f,39.30409f,39.279617f,39.25526f,39.231014f,39.20688f,39.182854f,39.15894f,39.135128f,39.111427f,39.08783f,39.06434f,39.04095f,39.01767f,38.994488f,38.97141f,38.948433f,38.925556f,38.902775f,38.880096f,38.857517f,38.835033f,38.812645f,38.79035f,38.768154f,38.74605f,38.72404f,38.702126f,38.6803f,38.65857f,38.636925f,38.61537f,38.59391f,38.572533f,38.551247f,38.53005f,38.508934f,38.487907f,38.466965f,38.44611f,38.42534f,38.404648f,38.38404f,38.363518f,38.343075f,38.322716f,38.302433f,38.282234f,38.26211f,38.24207f,38.222107f,38.20222f,38.18241f,38.162678f,38.14302f,38.12344f,38.103935f,38.084503f,38.065144f,38.04586f,38.02665f,38.00751f,37.988445f,37.969448f,37.950523f,37.93167f,37.912888f,37.894173f,37.875526f,37.85695f,37.83844f,37.82f,37.80163f,37.78332f,37.76508f,37.746906f,37.728794f,37.71075f,37.692772f,37.674854f,37.657005f,37.639217f,37.62149f,37.60383f,37.586227f,37.568687f,37.55121f,37.533794f,37.516438f,37.49914f,37.481903f,37.46473f,37.44761f,37.43055f,37.41355f,37.396606f,37.379723f,37.362892f,37.346123f,37.329407f,37.312748f,37.296146f,37.2796f,37.263103f,37.246666f,37.230286f,37.213955f,37.19768f,37.181458f,37.16529f,37.149174f,37.13311f,37.1171f,37.10114f,37.08523f,37.069374f,37.05357f,37.037815f,37.02211f,37.006454f,36.990852f,36.975296f,36.95979f,36.944332f,36.928925f,36.913563f,36.89825f,36.882988f,36.86777f,36.8526f,36.83748f,36.822403f,36.807373f,36.79239f,36.777454f,36.76256f,36.747715f,36.732914f,36.718155f,36.703445f,36.688778f,36.674152f,36.659576f,36.64504f,36.630547f,36.616096f,36.601692f,36.587326f,36.573006f,36.558727f,36.54449f,36.530296f,36.516144f,36.50203f,36.48796f,36.47393f,36.459938f,36.44599f,36.43208f,36.418213f,36.404385f,36.390594f,36.376842f,36.363132f,36.34946f,36.335827f,36.32223f,36.308678f,36.29516f,36.281677f,36.268234f,36.25483f,36.241463f,36.22813f,36.21484f,36.201584f,36.188362f,36.17518f,36.162033f,36.14892f,36.135845f,36.122807f,36.109802f,36.096832f,36.083897f,36.071f,36.058136f,36.045307f,36.03251f,36.01975f,36.007023f,35.99433f,35.98167f,35.969048f,35.956455f,35.943897f,35.93137f,35.91888f,35.906418f,35.893993f,35.881596f,35.869236f,35.856907f,35.84461f,35.83234f,35.820107f,35.807903f,35.79573f,35.783592f,35.77148f,35.759403f,35.747356f,35.73534f,35.723354f,35.7114f,35.69947f,35.687576f,35.67571f,35.663876f,35.65207f,35.640293f,35.628544f,35.616825f,35.605137f,35.593475f,35.581844f,35.570244f,35.55867f,35.547123f,35.535606f,35.52412f,35.512657f,35.501225f,35.48982f,35.478443f,35.467094f,35.455772f,35.444477f,35.43321f,35.421967f,35.410755f,35.399567f,35.388405f,35.377274f,35.366165f,35.355083f,35.34403f,35.333f,35.322f,35.311024f,35.30007f,35.289146f,35.278248f,35.267372f,35.256523f,35.2457f,35.2349f,35.22413f,35.213383f,35.202656f,35.19196f,35.181286f,35.170635f,35.160007f,35.149406f,35.138832f,35.128277f,35.11775f,35.107243f,35.096764f,35.086304f,35.07587f,35.06546f,35.055073f,35.04471f,35.034367f,35.024048f,35.013756f,35.003483f,34.993233f,34.983006f,34.9728f,34.96262f,34.95246f,34.942326f,34.932213f,34.92212f,34.91205f,34.902f,34.891975f,34.88197f,34.871986f,34.862026f,34.852085f,34.842167f,34.83227f,34.822395f,34.812542f,34.802708f,34.792892f,34.783104f,34.77333f,34.76358f,34.753853f,34.744144f,34.734455f,34.72479f,34.71514f,34.705513f,34.695908f,34.68632f,34.676754f,34.667206f,34.65768f,34.648174f,34.638687f,34.62922f,34.61977f,34.61034f,34.600933f,34.59154f,34.582172f,34.572823f,34.563488f,34.554176f,34.54488f,34.535606f,34.52635f,34.517113f,34.507893f,34.49869f,34.48951f,34.480347f,34.471203f,34.46208f,34.45297f,34.44388f,34.434807f,34.42575f,34.416718f,34.4077f,34.3987f,34.389717f,34.380753f,34.371807f,34.362877f,34.353966f,34.34507f,34.336193f,34.327335f,34.318493f,34.30967f,34.30086f,34.29207f,34.2833f,34.27454f,34.2658f,34.25708f,34.24837f,34.239685f,34.23101f,34.222355f,34.213715f,34.205093f,34.196487f,34.187897f,34.17932f,34.170765f,34.162224f,34.153698f,34.14519f,34.1367f,34.12822f,34.11976f,34.111317f,34.102886f,34.09447f,34.086075f,34.077694f,34.069324f,34.060974f,34.05264f,34.04432f,34.036015f,34.027725f,34.01945f,34.011192f,34.00295f,33.99472f,33.986507f,33.97831f,33.970123f,33.961956f,33.9538f,33.945663f,33.93754f,33.92943f,33.921333f,33.913254f,33.90519f,33.897137f,33.8891f,33.881077f,33.87307f,33.865078f,33.857098f,33.849133f,33.841183f,33.833244f,33.82532f,33.817413f,33.80952f,33.80164f,33.79377f,33.78592f,33.77808f,33.770252f,33.762444f,33.754646f,33.74686f,33.73909f,33.73133f,33.723587f,33.71586f,33.70814f,33.700436f,33.69275f,33.68507f,33.677406f,33.669758f,33.662117f,33.654495f,33.646885f,33.639286f,33.6317f,33.624126f,33.61657f,33.60902f,33.60149f,33.593967f,33.58646f,33.578964f,33.57148f,33.56401f,33.556553f,33.549107f,33.541676f,33.534256f,33.526848f,33.51945f,33.51207f,33.5047f,33.49734f,33.489994f,33.48266f,33.475338f,33.46803f,33.46073f,33.453445f,33.44617f,33.43891f,33.43166f,33.424423f,33.417194f,33.40998f,33.40278f,33.395588f,33.38841f,33.38124f,33.37409f,33.366943f,33.35981f,33.352688f,33.34558f,33.33848f,33.331394f,33.32432f,33.317257f,33.310204f,33.30316f,33.29613f,33.289116f,33.28211f,33.275112f,33.268124f,33.26115f,33.25419f,33.247234f,33.240295f,33.233364f,33.226444f,33.219536f,33.21264f,33.205753f,33.19888f,33.192013f,33.185158f,33.178314f,33.171482f,33.16466f,33.15785f,33.151047f,33.144257f,33.137478f,33.130707f,33.123947f,33.1172f,33.110462f,33.103733f,33.097015f,33.09031f,33.08361f,33.076927f,33.070248f,33.063583f,33.056927f,33.05028f,33.043644f,33.037018f,33.030403f,33.0238f,33.017204f,33.010616f,33.00404f,32.997475f,32.99092f,32.98437f,32.977837f,32.97131f,32.964794f,32.958286f,32.95179f,32.9453f,32.938824f,32.932354f,32.925896f,32.91945f,32.91301f,32.906578f,32.900158f,32.893745f,32.887344f,32.88095f,32.87457f,32.868195f,32.86183f,32.855476f,32.849133f,32.842796f,32.836468f,32.83015f,32.82384f,32.817543f,32.811253f,32.80497f,32.7987f,32.792435f,32.786182f,32.779938f,32.7737f,32.767475f,32.761257f,32.755047f,32.748848f,32.742657f,32.736473f,32.7303f,32.724136f,32.71798f,32.71183f,32.705692f,32.699562f,32.693443f,32.68733f,32.681225f,32.67513f,32.669044f,32.662964f,32.656895f,32.650833f,32.644783f,32.638737f,32.6327f,32.626675f,32.620655f,32.614647f,32.608646f,32.60265f,32.596664f,32.59069f,32.58472f,32.57876f,32.572807f,32.566864f,32.56093f,32.555f,32.54908f,32.54317f,32.537266f,32.531372f,32.525486f,32.519604f,32.513733f,32.507874f,32.502018f,32.49617f,32.49033f,32.4845f,32.478676f,32.472862f,32.467052f,32.461254f,32.455463f,32.44968f,32.443905f,32.438133f,32.432373f,32.42662f,32.420876f,32.41514f,32.40941f,32.403687f,32.397972f,32.392265f,32.386566f,32.380875f,32.37519f,32.369514f,32.363846f,32.358185f,32.35253f,32.34688f,32.341244f,32.335613f,32.329987f,32.32437f,32.31876f,32.31316f,32.307564f,32.30198f,32.2964f,32.290825f,32.28526f,32.2797f,32.27415f,32.268604f,32.26307f,32.257538f,32.252018f,32.246502f,32.240993f,32.235493f,32.23f,32.22451f,32.219032f,32.21356f,32.20809f,32.202633f,32.19718f,32.19174f,32.186302f,32.18087f,32.175446f,32.17003f,32.16462f,32.159214f,32.15382f,32.14843f,32.143047f,32.137672f,32.1323f,32.126938f,32.121586f,32.116234f,32.110893f,32.105556f,32.10023f,32.094906f,32.089592f,32.084282f,32.07898f,32.073685f,32.068398f,32.063114f,32.05784f,32.05257f,32.047306f,32.042053f,32.036804f,32.03156f,32.02632f,32.02109f,32.01587f,32.01065f,32.00544f,32.000237f,31.995039f,31.989847f,31.984663f,31.979485f,31.974312f,31.969145f,31.963985f,31.958832f,31.953684f,31.948544f,31.943409f,31.93828f,31.933159f,31.928041f,31.922932f,31.917828f,31.912731f,31.907639f,31.902554f,31.897474f,31.892403f,31.887335f,31.882275f,31.87722f,31.872171f,31.867128f,31.862091f,31.857061f,31.852036f,31.847017f,31.842005f,31.836998f,31.831997f,31.827002f,31.822014f,31.81703f,31.812054f,31.807081f,31.802116f,31.797157f,31.792204f,31.787256f,31.782314f,31.777378f,31.772448f,31.767523f,31.762604f,31.75769f,31.752783f,31.74788f,31.742985f,31.738094f,31.733212f,31.728333f,31.72346f,31.718592f,31.71373f,31.708874f,31.704021f,31.699177f,31.694338f,31.689505f,31.684675f,31.679853f,31.675035f,31.670223f,31.665419f,31.660618f,31.655823f,31.651031f,31.646248f,31.64147f,31.636696f,31.63193f,31.627167f,31.62241f,31.617657f,31.612911f,31.608171f,31.603436f,31.598705f,31.59398f,31.58926f,31.584547f,31.579838f,31.575134f,31.570436f,31.565744f,31.561056f,31.556374f,31.551697f,31.547024f,31.542358f,31.537697f,31.533041f,31.528389f,31.523743f,31.519102f,31.514467f,31.509836f,31.50521f,31.500591f,31.495977f,31.491367f,31.486761f,31.482162f,31.477568f,31.472979f,31.468393f,31.463814f,31.45924f,31.45467f,31.450106f,31.445547f,31.440992f,31.436443f,31.431898f,31.427359f,31.422825f,31.418295f,31.41377f,31.40925f,31.404736f,31.400225f,31.395721f,31.39122f,31.386724f,31.382235f,31.377748f,31.373268f,31.368792f,31.36432f,31.359856f,31.355394f,31.350937f,31.346485f,31.342037f,31.337595f,31.333158f,31.328726f,31.324297f,31.319874f,31.315454f,31.31104f,31.306631f,31.302227f,31.297827f,31.293432f,31.289042f,31.284657f,31.280275f,31.275898f,31.271526f,31.267159f,31.262796f,31.258438f,31.254084f,31.249735f,31.24539f,31.24105f,31.236715f,31.232384f,31.228058f,31.223736f,31.21942f,31.215107f,31.210798f,31.206495f,31.202196f,31.1979f,31.193611f,31.189325f,31.185043f,31.180767f,31.176493f,31.172226f,31.167961f,31.163702f,31.159447f,31.155197f,31.150951f,31.14671f,31.142471f,31.138239f,31.13401f,31.129786f,31.125566f,31.12135f,31.117138f,31.112932f,31.108728f,31.10453f,31.100336f,31.096146f,31.09196f};
class mydspSIG11 {
	
  private:
	
	int fmydspSIG11Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG11() {
		return 0;
	}
	int getNumOutputsmydspSIG11() {
		return 1;
	}
	
	void instanceInitmydspSIG11(int sample_rate) {
		fmydspSIG11Wave0_idx = 0;
	}
	
	void fillmydspSIG11(int count, float* table) {
		for (int i12 = 0; i12 < count; i12 = i12 + 1) {
			table[i12] = fmydspSIG11Wave0[fmydspSIG11Wave0_idx];
			fmydspSIG11Wave0_idx = (1 + fmydspSIG11Wave0_idx) % 2001;
		}
	}

};

static mydspSIG11* newmydspSIG11() { return (mydspSIG11*)new mydspSIG11(); }
static void deletemydspSIG11(mydspSIG11* dsp) { delete dsp; }

static float mydsp_faustpower2_f(float value) {
	return value * value;
}
static float mydsp_faustpower3_f(float value) {
	return value * value * value;
}
static float ftbl0mydspSIG0[2001];
static float ftbl1mydspSIG1[2001];
static float ftbl2mydspSIG2[2001];
static float ftbl3mydspSIG3[2001];
static float ftbl4mydspSIG4[2001];
static float ftbl5mydspSIG5[2001];
static float ftbl6mydspSIG6[2001];
static float ftbl7mydspSIG7[2001];
static float ftbl8mydspSIG8[2001];
static float ftbl9mydspSIG9[2001];
static float ftbl10mydspSIG10[2001];
static float ftbl11mydspSIG11[2001];

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fCheckbox0;
	int fSampleRate;
	float fConst0;
	float fConst1;
	FAUSTFLOAT fHslider0;
	float fConst2;
	float fRec0[2];
	FAUSTFLOAT fHslider1;
	float fRec1[2];
	float fConst3;
	FAUSTFLOAT fCheckbox1;
	float fRec3[2];
	FAUSTFLOAT fEntry0;
	FAUSTFLOAT fCheckbox2;
	float fRec4[2];
	float fConst4;
	FAUSTFLOAT fVslider0;
	FAUSTFLOAT fVslider1;
	float fConst5;
	float fConst6;
	FAUSTFLOAT fVslider2;
	float fConst7;
	float fConst8;
	float fConst9;
	float fConst10;
	float fConst11;
	float fConst12;
	float fConst13;
	float fConst14;
	float fConst15;
	float fConst16;
	float fConst17;
	float fConst18;
	float fConst19;
	float fConst20;
	float fConst21;
	float fConst22;
	float fConst23;
	float fConst24;
	float fConst25;
	FAUSTFLOAT fHslider2;
	float fRec11[2];
	FAUSTFLOAT fEntry1;
	FAUSTFLOAT fHslider3;
	float fRec13[2];
	float fRec12[2];
	FAUSTFLOAT fCheckbox3;
	float fRec14[2];
	float fConst26;
	FAUSTFLOAT fEntry2;
	float fConst27;
	float fConst28;
	float fConst29;
	float fConst30;
	float fConst31;
	float fConst32;
	float fConst33;
	float fRec18[2];
	float fConst34;
	float fConst35;
	float fConst36;
	float fConst37;
	float fConst38;
	FAUSTFLOAT fHslider4;
	float fRec20[2];
	float fConst39;
	float fConst40;
	float fConst41;
	float fConst42;
	float fVec0[2];
	float fRec23[2];
	float fRec22[2];
	float fConst43;
	float fRec21[3];
	float fConst44;
	float fConst45;
	float fConst46;
	float fRec19[2];
	FAUSTFLOAT fHbargraph0;
	float fConst47;
	float fConst48;
	float fRec17[3];
	float fConst49;
	float fRec16[2];
	float fRec26[2];
	float fConst50;
	float fConst51;
	float fRec25[3];
	float fRec24[2];
	float fRec29[2];
	float fConst52;
	float fConst53;
	float fRec28[3];
	float fRec27[2];
	float fRec32[2];
	float fConst54;
	float fConst55;
	float fRec31[3];
	float fRec30[2];
	float fConst56;
	float fRec35[2];
	float fConst57;
	float fConst58;
	float fRec34[3];
	float fRec33[2];
	float fRec38[2];
	float fConst59;
	float fConst60;
	float fRec37[3];
	float fRec36[2];
	float fRec15[2];
	FAUSTFLOAT fHbargraph1;
	FAUSTFLOAT fEntry3;
	float fConst61;
	float fConst62;
	float fConst63;
	float fRec42[2];
	float fConst64;
	float fConst65;
	float fConst66;
	float fVec1[2];
	float fRec43[2];
	float fConst67;
	float fConst68;
	float fRec41[3];
	float fRec40[2];
	float fRec46[2];
	float fConst69;
	float fConst70;
	float fRec45[3];
	float fRec44[2];
	float fRec49[2];
	float fConst71;
	float fConst72;
	float fRec48[3];
	float fRec47[2];
	float fRec52[2];
	float fConst73;
	float fConst74;
	float fRec51[3];
	float fRec50[2];
	float fRec55[2];
	float fConst75;
	float fConst76;
	float fRec54[3];
	float fRec53[2];
	float fRec58[2];
	float fConst77;
	float fConst78;
	float fRec57[3];
	float fRec56[2];
	float fRec39[2];
	FAUSTFLOAT fHbargraph2;
	FAUSTFLOAT fEntry4;
	float fConst79;
	float fConst80;
	float fConst81;
	float fRec61[2];
	float fVec2[2];
	float fRec62[2];
	float fRec60[3];
	float fRec59[2];
	float fRec65[2];
	float fRec64[3];
	float fRec63[2];
	float fRec68[2];
	float fRec67[3];
	float fRec66[2];
	float fRec71[2];
	float fRec70[3];
	float fRec69[2];
	float fRec74[2];
	float fRec73[3];
	float fRec72[2];
	float fRec77[2];
	float fRec76[3];
	float fRec75[2];
	float fVec3[2];
	float fRec10[2];
	float fConst82;
	float fConst83;
	float fRec9[3];
	float fRec79[2];
	float fRec78[3];
	float fConst84;
	float fRec8[3];
	float fConst85;
	float fConst86;
	float fConst87;
	float fVec4[2];
	float fRec7[2];
	float fConst88;
	float fConst89;
	float fConst90;
	float fRec6[3];
	float fConst91;
	float fConst92;
	float fRec81[2];
	float fRec80[3];
	float fRec5[4];
	float fRec82[4];
	float fRec83[4];
	float fRec84[4];
	FAUSTFLOAT fHslider5;
	float fRec88[2];
	float fConst93;
	float fConst94;
	FAUSTFLOAT fHslider6;
	FAUSTFLOAT fHslider7;
	float fRec89[2];
	float fConst95;
	float fConst96;
	float fConst97;
	float fRec91[3];
	float fConst98;
	float fRec90[3];
	FAUSTFLOAT fHslider8;
	float fRec92[2];
	float fConst99;
	float fConst100;
	float fRec93[2];
	FAUSTFLOAT fHslider9;
	float fRec94[2];
	FAUSTFLOAT fHslider10;
	float fRec95[2];
	float fVec5[2];
	float fRec87[2];
	float fRec86[2];
	float fRec85[2];
	FAUSTFLOAT fHslider11;
	float fRec96[2];
	float fVec6[2];
	float fRec2[2];
	FAUSTFLOAT fHslider12;
	float fRec97[2];
	FAUSTFLOAT fHslider13;
	float fRec98[2];
	FAUSTFLOAT fEntry5;
	float fConst101;
	float fConst102;
	float fConst103;
	float fConst104;
	float fConst105;
	float fConst106;
	float fConst107;
	float fConst108;
	float fConst109;
	float fConst110;
	float fConst111;
	float fConst112;
	float fConst113;
	float fConst114;
	float fRec110[2];
	float fConst115;
	float fRec109[2];
	int IOTA0;
	float fVec7[32768];
	float fConst116;
	int iConst117;
	float fVec8[8192];
	int iConst118;
	float fVec9[2048];
	int iConst119;
	float fRec107[2];
	float fConst120;
	float fConst121;
	float fConst122;
	float fConst123;
	float fConst124;
	float fConst125;
	float fConst126;
	float fConst127;
	float fConst128;
	float fConst129;
	float fRec114[2];
	float fConst130;
	float fRec113[2];
	float fVec10[32768];
	float fConst131;
	int iConst132;
	float fVec11[4096];
	int iConst133;
	float fRec111[2];
	float fConst134;
	float fConst135;
	float fConst136;
	float fConst137;
	float fConst138;
	float fConst139;
	float fConst140;
	float fConst141;
	float fConst142;
	float fConst143;
	float fRec118[2];
	float fConst144;
	float fRec117[2];
	float fVec12[16384];
	float fConst145;
	int iConst146;
	float fVec13[4096];
	int iConst147;
	float fRec115[2];
	float fConst148;
	float fConst149;
	float fConst150;
	float fConst151;
	float fConst152;
	float fConst153;
	float fConst154;
	float fConst155;
	float fConst156;
	float fConst157;
	float fRec122[2];
	float fConst158;
	float fRec121[2];
	float fVec14[32768];
	float fConst159;
	int iConst160;
	float fVec15[4096];
	int iConst161;
	float fRec119[2];
	float fConst162;
	float fConst163;
	float fConst164;
	float fConst165;
	float fConst166;
	float fConst167;
	float fConst168;
	float fConst169;
	float fConst170;
	float fConst171;
	float fRec126[2];
	float fConst172;
	float fRec125[2];
	float fVec16[16384];
	float fConst173;
	int iConst174;
	float fVec17[2048];
	int iConst175;
	float fRec123[2];
	float fConst176;
	float fConst177;
	float fConst178;
	float fConst179;
	float fConst180;
	float fConst181;
	float fConst182;
	float fConst183;
	float fConst184;
	float fConst185;
	float fRec130[2];
	float fConst186;
	float fRec129[2];
	float fVec18[16384];
	float fConst187;
	int iConst188;
	float fVec19[4096];
	int iConst189;
	float fRec127[2];
	float fConst190;
	float fConst191;
	float fConst192;
	float fConst193;
	float fConst194;
	float fConst195;
	float fConst196;
	float fConst197;
	float fConst198;
	float fConst199;
	float fRec134[2];
	float fConst200;
	float fRec133[2];
	float fVec20[16384];
	float fConst201;
	int iConst202;
	float fVec21[4096];
	int iConst203;
	float fRec131[2];
	float fConst204;
	float fConst205;
	float fConst206;
	float fConst207;
	float fConst208;
	float fConst209;
	float fConst210;
	float fConst211;
	float fConst212;
	float fConst213;
	float fRec138[2];
	float fConst214;
	float fRec137[2];
	float fVec22[16384];
	float fConst215;
	int iConst216;
	float fVec23[2048];
	int iConst217;
	float fRec135[2];
	float fRec99[3];
	float fRec100[3];
	float fRec101[3];
	float fRec102[3];
	float fRec103[3];
	float fRec104[3];
	float fRec105[3];
	float fRec106[3];
	float fConst218;
	float fConst219;
	float fConst220;
	float fConst221;
	float fConst222;
	float fConst223;
	float fConst224;
	float fConst225;
	int iConst226;
	int iConst227;
	int iConst228;
	int iConst229;
	int iConst230;
	int iConst231;
	int iConst232;
	int iConst233;
	float fVec24[2048];
	int iConst234;
	float fVec25[4096];
	int iConst235;
	float fVec26[4096];
	int iConst236;
	float fVec27[8192];
	int iConst237;
	float fVec28[4096];
	int iConst238;
	float fVec29[8192];
	int iConst239;
	float fVec30[4096];
	int iConst240;
	float fVec31[8192];
	int iConst241;
	float fVec32[2048];
	float fConst242;
	int iConst243;
	float fVec33[4096];
	int iConst244;
	float fVec34[4096];
	int iConst245;
	float fVec35[4096];
	int iConst246;
	float fVec36[4096];
	int iConst247;
	float fVec37[4096];
	int iConst248;
	float fVec38[4096];
	int iConst249;
	float fVec39[8192];
	int iConst250;
	float fVec40[2048];
	float fVec41[16384];
	float fConst251;
	int iConst252;
	int iConst253;
	float fRec148[2];
	float fRec140[2];
	float fVec42[4096];
	float fVec43[16384];
	int iConst254;
	int iConst255;
	float fRec149[2];
	float fRec141[2];
	float fVec44[4096];
	int iConst256;
	float fVec45[16384];
	int iConst257;
	int iConst258;
	float fRec150[2];
	float fRec142[2];
	float fVec46[4096];
	int iConst259;
	float fVec47[16384];
	int iConst260;
	int iConst261;
	float fRec151[2];
	float fRec143[2];
	float fVec48[4096];
	int iConst262;
	float fVec49[16384];
	int iConst263;
	int iConst264;
	float fRec152[2];
	float fRec144[2];
	float fVec50[8192];
	int iConst265;
	float fVec51[16384];
	int iConst266;
	int iConst267;
	float fRec153[2];
	float fRec145[2];
	float fVec52[8192];
	int iConst268;
	float fVec53[16384];
	int iConst269;
	int iConst270;
	float fRec154[2];
	float fRec146[2];
	float fVec54[8192];
	int iConst271;
	float fVec55[16384];
	int iConst272;
	int iConst273;
	float fRec155[2];
	float fRec147[2];
	float fVec56[2];
	float fRec139[2];
	float fRec165[2048];
	int iConst274;
	float fRec166[2];
	float fVec57[8192];
	int iConst275;
	int iConst276;
	float fRec164[2];
	float fRec168[2];
	float fVec58[8192];
	int iConst277;
	int iConst278;
	float fRec167[2];
	float fRec170[2];
	float fVec59[8192];
	int iConst279;
	int iConst280;
	float fRec169[2];
	float fRec172[2];
	float fVec60[8192];
	int iConst281;
	int iConst282;
	float fRec171[2];
	float fRec174[2];
	float fVec61[8192];
	int iConst283;
	int iConst284;
	float fRec173[2];
	float fRec176[2];
	float fVec62[8192];
	int iConst285;
	int iConst286;
	float fRec175[2];
	float fRec178[2];
	float fVec63[8192];
	int iConst287;
	int iConst288;
	float fRec177[2];
	float fRec180[2];
	float fVec64[8192];
	int iConst289;
	int iConst290;
	float fRec179[2];
	float fVec65[2048];
	int iConst291;
	int iConst292;
	float fRec162[2];
	float fVec66[2048];
	int iConst293;
	int iConst294;
	float fRec160[2];
	float fVec67[2048];
	int iConst295;
	int iConst296;
	float fRec158[2];
	float fVec68[1024];
	int iConst297;
	int iConst298;
	float fRec156[2];
	float fRec190[2];
	float fVec69[8192];
	int iConst299;
	float fRec189[2];
	float fRec192[2];
	float fVec70[8192];
	int iConst300;
	float fRec191[2];
	float fRec194[2];
	float fVec71[8192];
	int iConst301;
	float fRec193[2];
	float fRec196[2];
	float fVec72[8192];
	int iConst302;
	float fRec195[2];
	float fRec198[2];
	float fVec73[8192];
	int iConst303;
	float fRec197[2];
	float fRec200[2];
	float fVec74[8192];
	int iConst304;
	float fRec199[2];
	float fRec202[2];
	float fVec75[8192];
	int iConst305;
	float fRec201[2];
	float fRec204[2];
	float fVec76[8192];
	int iConst306;
	float fRec203[2];
	float fVec77[2048];
	int iConst307;
	float fRec187[2];
	float fVec78[2048];
	int iConst308;
	float fRec185[2];
	float fVec79[2048];
	int iConst309;
	float fRec183[2];
	float fVec80[1024];
	int iConst310;
	float fRec181[2];
	FAUSTFLOAT fEntry6;
	float fVec81[1024];
	float fVec82[1024];
	float fVec83[1024];
	
 public:
	mydsp() {
	}
	
	mydsp(const mydsp&) = default;
	
	virtual ~mydsp() = default;
	
	mydsp& operator=(const mydsp&) = default;
	
	void metadata(Meta* m) { 
		m->declare("IFCguitarixTubes.dsp/author", "Generated automatically from Guitarix C++ arrays");
		m->declare("IFCguitarixTubes.dsp/license", "LGPL");
		m->declare("IFCguitarixTubes.dsp/name", "Tube Emulation Library (100% Faust)");
		m->declare("IFCpowerAmp.dsp/author", "Michel Buffa and Jerome Lebrun");
		m->declare("IFCpowerAmp.dsp/license", "LGPL");
		m->declare("IFCpowerAmp.dsp/name", "IFCpowerAmp");
		m->declare("IFCpowerAmp.dsp/version", "1.0");
		m->declare("IFCreverbMulti.dsp/author", "Michel Buffa");
		m->declare("IFCreverbMulti.dsp/license", "LGPL");
		m->declare("IFCreverbMulti.dsp/name", "IFCreverbMulti");
		m->declare("IFCreverbMulti.dsp/version", "1.0");
		m->declare("IFCtonestackMulti.dsp/author", "Michel Buffa / based on Guitarix tonestacks.lib");
		m->declare("IFCtonestackMulti.dsp/license", "LGPL");
		m->declare("IFCtonestackMulti.dsp/name", "IFCtonestack");
		m->declare("IFCtonestackMulti.dsp/version", "1.0");
		m->declare("aanl.lib/ADAA1:author", "Dario Sanfilippo");
		m->declare("aanl.lib/ADAA1:copyright", "Copyright (C) 2021 Dario Sanfilippo     <sanfilippo.dario@gmail.com>");
		m->declare("aanl.lib/ADAA1:license", "MIT License");
		m->declare("aanl.lib/hardclip:author", "Dario Sanfilippo");
		m->declare("aanl.lib/hardclip:copyright", "Copyright (C) 2021 Dario Sanfilippo     <sanfilippo.dario@gmail.com>");
		m->declare("aanl.lib/hardclip:license", "MIT License");
		m->declare("aanl.lib/name", "Faust Antialiased Nonlinearities");
		m->declare("aanl.lib/version", "1.4.2");
		m->declare("analyzers.lib/name", "Faust Analyzer Library");
		m->declare("analyzers.lib/version", "1.3.0");
		m->declare("author", "Michel Buffa & Jerome Lebrun");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/selectoutn:author", "Vince");
		m->declare("basics.lib/version", "1.22.0");
		m->declare("category", "Amp");
		m->declare("compile_options", "-a /opt/homebrew/Cellar/faust/2.83.1_1/share/faust/sndfile.cpp -lang cpp -i -fpga-mem-th 4 -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "1.2.0");
		m->declare("filename", "IFCampSim.dsp");
		m->declare("filters.lib/allpass_comb:author", "Julius O. Smith III");
		m->declare("filters.lib/allpass_comb:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/allpass_comb:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/dcblocker:author", "Julius O. Smith III");
		m->declare("filters.lib/dcblocker:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/dcblocker:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/filterbank:author", "Julius O. Smith III");
		m->declare("filters.lib/filterbank:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/filterbank:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/fir:author", "Julius O. Smith III");
		m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/highpass:author", "Julius O. Smith III");
		m->declare("filters.lib/highpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/highshelf:author", "Julius O. Smith III");
		m->declare("filters.lib/highshelf:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/highshelf:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/iir:author", "Julius O. Smith III");
		m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/low_shelf:author", "Julius O. Smith III");
		m->declare("filters.lib/low_shelf:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/low_shelf:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowshelf:author", "Julius O. Smith III");
		m->declare("filters.lib/lowshelf:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowshelf:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/peak_eq:author", "Julius O. Smith III");
		m->declare("filters.lib/peak_eq:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/peak_eq:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/peak_eq_cq:author", "Julius O. Smith III");
		m->declare("filters.lib/peak_eq_cq:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/peak_eq_cq:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/pole:author", "Julius O. Smith III");
		m->declare("filters.lib/pole:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/pole:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf1:author", "Julius O. Smith III");
		m->declare("filters.lib/tf1:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf1:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf1s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf1s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf1s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "1.7.1");
		m->declare("filters.lib/zero:author", "Julius O. Smith III");
		m->declare("filters.lib/zero:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/zero:license", "MIT-style STK-4.3 license");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.9.0");
		m->declare("name", "guitar tube amp sim 100% FAUST");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "1.3.0");
		m->declare("reverbs.lib/mono_freeverb:author", "Romain Michon");
		m->declare("reverbs.lib/name", "Faust Reverb Library");
		m->declare("reverbs.lib/springreverb:author", "Daniel Leonov");
		m->declare("reverbs.lib/stereo_freeverb:author", "Romain Michon");
		m->declare("reverbs.lib/version", "1.5.1");
		m->declare("routes.lib/hadamard:author", "Remy Muller, revised by Romain Michon");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "1.2.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "1.6.0");
		m->declare("version", "0.1");
		m->declare("webaudio.lib/author", "GRAME");
		m->declare("webaudio.lib/copyright", "GRAME");
		m->declare("webaudio.lib/license", "LGPL with exception");
		m->declare("webaudio.lib/name", "Faust WebAudio Filters Library");
		m->declare("webaudio.lib/version", "1.1.0");
	}

	virtual int getNumInputs() {
		return 1;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
		mydspSIG0* sig0 = newmydspSIG0();
		sig0->instanceInitmydspSIG0(sample_rate);
		sig0->fillmydspSIG0(2001, ftbl0mydspSIG0);
		mydspSIG1* sig1 = newmydspSIG1();
		sig1->instanceInitmydspSIG1(sample_rate);
		sig1->fillmydspSIG1(2001, ftbl1mydspSIG1);
		mydspSIG2* sig2 = newmydspSIG2();
		sig2->instanceInitmydspSIG2(sample_rate);
		sig2->fillmydspSIG2(2001, ftbl2mydspSIG2);
		mydspSIG3* sig3 = newmydspSIG3();
		sig3->instanceInitmydspSIG3(sample_rate);
		sig3->fillmydspSIG3(2001, ftbl3mydspSIG3);
		mydspSIG4* sig4 = newmydspSIG4();
		sig4->instanceInitmydspSIG4(sample_rate);
		sig4->fillmydspSIG4(2001, ftbl4mydspSIG4);
		mydspSIG5* sig5 = newmydspSIG5();
		sig5->instanceInitmydspSIG5(sample_rate);
		sig5->fillmydspSIG5(2001, ftbl5mydspSIG5);
		mydspSIG6* sig6 = newmydspSIG6();
		sig6->instanceInitmydspSIG6(sample_rate);
		sig6->fillmydspSIG6(2001, ftbl6mydspSIG6);
		mydspSIG7* sig7 = newmydspSIG7();
		sig7->instanceInitmydspSIG7(sample_rate);
		sig7->fillmydspSIG7(2001, ftbl7mydspSIG7);
		mydspSIG8* sig8 = newmydspSIG8();
		sig8->instanceInitmydspSIG8(sample_rate);
		sig8->fillmydspSIG8(2001, ftbl8mydspSIG8);
		mydspSIG9* sig9 = newmydspSIG9();
		sig9->instanceInitmydspSIG9(sample_rate);
		sig9->fillmydspSIG9(2001, ftbl9mydspSIG9);
		mydspSIG10* sig10 = newmydspSIG10();
		sig10->instanceInitmydspSIG10(sample_rate);
		sig10->fillmydspSIG10(2001, ftbl10mydspSIG10);
		mydspSIG11* sig11 = newmydspSIG11();
		sig11->instanceInitmydspSIG11(sample_rate);
		sig11->fillmydspSIG11(2001, ftbl11mydspSIG11);
		deletemydspSIG0(sig0);
		deletemydspSIG1(sig1);
		deletemydspSIG2(sig2);
		deletemydspSIG3(sig3);
		deletemydspSIG4(sig4);
		deletemydspSIG5(sig5);
		deletemydspSIG6(sig6);
		deletemydspSIG7(sig7);
		deletemydspSIG8(sig8);
		deletemydspSIG9(sig9);
		deletemydspSIG10(sig10);
		deletemydspSIG11(sig11);
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<float>(1.92e+05f, std::max<float>(1.0f, static_cast<float>(fSampleRate)));
		fConst1 = 44.1f / fConst0;
		fConst2 = 1.0f - fConst1;
		fConst3 = 1e+01f / fConst0;
		fConst4 = 2.0f * fConst0;
		fConst5 = mydsp_faustpower2_f(fConst4);
		fConst6 = mydsp_faustpower3_f(fConst4);
		fConst7 = 3.0f * fConst6;
		fConst8 = std::tan(12566.371f / fConst0);
		fConst9 = 1.0f / fConst8;
		fConst10 = 1.0f / ((fConst9 + 1.0f) / fConst8 + 1.0f);
		fConst11 = 1.0f / (fConst9 + 1.0f);
		fConst12 = 1.0f - fConst9;
		fConst13 = std::tan(12390.127f / fConst0);
		fConst14 = 1.0f / fConst13;
		fConst15 = fConst0 * std::sin(24780.254f / fConst0);
		fConst16 = 5272.3945f / fConst15;
		fConst17 = (fConst14 + fConst16) / fConst13 + 1.0f;
		fConst18 = 1.0f / fConst17;
		fConst19 = 2.0f * (1.0f - 1.0f / mydsp_faustpower2_f(fConst13));
		fConst20 = std::tan(628.31854f / fConst0);
		fConst21 = 1.0f / fConst20;
		fConst22 = 1.0f / ((fConst21 + 1.0f) / fConst20 + 1.0f);
		fConst23 = 1.0f / mydsp_faustpower2_f(fConst20);
		fConst24 = 1.0f / (fConst21 + 1.0f);
		fConst25 = 1.0f - fConst21;
		fConst26 = std::exp(-(2.0f / fConst0));
		fConst27 = std::tan(97.38937f / fConst0);
		fConst28 = 1.0f / fConst27;
		fConst29 = 1.0f / (fConst28 + 1.0f);
		fConst30 = 0.025f / fConst27;
		fConst31 = 1.0f / std::tan(270.17697f / fConst0);
		fConst32 = 1.0f / (fConst31 + 1.0f);
		fConst33 = 1.0f - fConst31;
		fConst34 = std::tan(3840.283f / fConst0);
		fConst35 = 1.0f / fConst34;
		fConst36 = fConst0 * std::sin(7680.566f / fConst0);
		fConst37 = 14770.318f / fConst36;
		fConst38 = 1.0f / ((fConst35 + fConst37) / fConst34 + 1.0f);
		fConst39 = 2.0f * (1.0f - 1.0f / mydsp_faustpower2_f(fConst34));
		fConst40 = 1.0f / std::tan(72.25663f / fConst0);
		fConst41 = 1.0f / (fConst40 + 1.0f);
		fConst42 = 1.0f - fConst40;
		fConst43 = (fConst35 - fConst37) / fConst34 + 1.0f;
		fConst44 = 19471.072f / fConst36;
		fConst45 = (fConst35 + fConst44) / fConst34 + 1.0f;
		fConst46 = (fConst35 - fConst44) / fConst34 + 1.0f;
		fConst47 = ftbl0mydspSIG0[2000];
		fConst48 = ftbl0mydspSIG0[0];
		fConst49 = 1.0f - fConst28;
		fConst50 = ftbl1mydspSIG1[2000];
		fConst51 = ftbl1mydspSIG1[0];
		fConst52 = ftbl2mydspSIG2[2000];
		fConst53 = ftbl2mydspSIG2[0];
		fConst54 = ftbl3mydspSIG3[2000];
		fConst55 = ftbl3mydspSIG3[0];
		fConst56 = 0.05f / fConst27;
		fConst57 = ftbl4mydspSIG4[2000];
		fConst58 = ftbl4mydspSIG4[0];
		fConst59 = ftbl5mydspSIG5[2000];
		fConst60 = ftbl5mydspSIG5[0];
		fConst61 = 1.0f / std::tan(414.69022f / fConst0);
		fConst62 = 1.0f / (fConst61 + 1.0f);
		fConst63 = 1.0f - fConst61;
		fConst64 = 1.0f / std::tan(20517.742f / fConst0);
		fConst65 = 1.0f / (fConst64 + 1.0f);
		fConst66 = 1.0f - fConst64;
		fConst67 = ftbl6mydspSIG6[2000];
		fConst68 = ftbl6mydspSIG6[0];
		fConst69 = ftbl7mydspSIG7[2000];
		fConst70 = ftbl7mydspSIG7[0];
		fConst71 = ftbl8mydspSIG8[2000];
		fConst72 = ftbl8mydspSIG8[0];
		fConst73 = ftbl9mydspSIG9[2000];
		fConst74 = ftbl9mydspSIG9[0];
		fConst75 = ftbl10mydspSIG10[2000];
		fConst76 = ftbl10mydspSIG10[0];
		fConst77 = ftbl11mydspSIG11[2000];
		fConst78 = ftbl11mydspSIG11[0];
		fConst79 = 1.0f / std::tan(609.469f / fConst0);
		fConst80 = 1.0f / (fConst79 + 1.0f);
		fConst81 = 1.0f - fConst79;
		fConst82 = (fConst21 + -1.0f) / fConst20 + 1.0f;
		fConst83 = 2.0f * (1.0f - fConst23);
		fConst84 = (fConst14 - fConst16) / fConst13 + 1.0f;
		fConst85 = 25322.578f / fConst15;
		fConst86 = (fConst14 + fConst85) / fConst13 + 1.0f;
		fConst87 = (fConst14 - fConst85) / fConst13 + 1.0f;
		fConst88 = (fConst9 + -1.0f) / fConst8 + 1.0f;
		fConst89 = mydsp_faustpower2_f(fConst8);
		fConst90 = 2.0f * (1.0f - 1.0f / fConst89);
		fConst91 = 0.31224823f / fConst89;
		fConst92 = 1.0f / (fConst8 * fConst17);
		fConst93 = 3.1415927f * std::max<float>(0.01f, std::min<float>(8004.6226f / fConst0, 0.99f));
		fConst94 = 0.5f * std::sin(fConst93);
		fConst95 = 3.1415927f * std::max<float>(0.01f, std::min<float>(4002.3113f / fConst0, 0.99f));
		fConst96 = 0.5f * std::sin(fConst95);
		fConst97 = 2.0f * std::cos(fConst95);
		fConst98 = 2.0f * std::cos(fConst93);
		fConst99 = std::exp(-(25.0f / fConst0));
		fConst100 = 1.0f - fConst99;
		fConst101 = std::floor(0.219991f * fConst0 + 0.5f);
		fConst102 = fConst101 / fConst0;
		fConst103 = std::exp(-(4.934111f * fConst102));
		fConst104 = std::cos(23876.104f / fConst0);
		fConst105 = mydsp_faustpower2_f(fConst103);
		fConst106 = 1.0f - fConst104 * fConst105;
		fConst107 = 1.0f - fConst105;
		fConst108 = std::sqrt(std::max<float>(0.0f, mydsp_faustpower2_f(fConst106) / mydsp_faustpower2_f(fConst107) + -1.0f));
		fConst109 = fConst106 / fConst107;
		fConst110 = fConst103 * (fConst108 + (1.0f - fConst109));
		fConst111 = std::exp(-(3.1398888f * fConst102)) / fConst103 + -1.0f;
		fConst112 = 1.0f / std::tan(942.4778f / fConst0);
		fConst113 = 1.0f / (fConst112 + 1.0f);
		fConst114 = 1.0f - fConst112;
		fConst115 = fConst109 - fConst108;
		fConst116 = std::floor(0.019123f * fConst0 + 0.5f);
		iConst117 = static_cast<int>(std::min<float>(16384.0f, std::max<float>(0.0f, fConst101 - fConst116)));
		iConst118 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, 0.028f * fConst0)));
		iConst119 = static_cast<int>(std::min<float>(1024.0f, std::max<float>(0.0f, fConst116 + -1.0f)));
		fConst120 = std::floor(0.256891f * fConst0 + 0.5f);
		fConst121 = fConst120 / fConst0;
		fConst122 = std::exp(-(4.934111f * fConst121));
		fConst123 = mydsp_faustpower2_f(fConst122);
		fConst124 = 1.0f - fConst104 * fConst123;
		fConst125 = 1.0f - fConst123;
		fConst126 = std::sqrt(std::max<float>(0.0f, mydsp_faustpower2_f(fConst124) / mydsp_faustpower2_f(fConst125) + -1.0f));
		fConst127 = fConst124 / fConst125;
		fConst128 = fConst122 * (fConst126 + (1.0f - fConst127));
		fConst129 = std::exp(-(3.1398888f * fConst121)) / fConst122 + -1.0f;
		fConst130 = fConst127 - fConst126;
		fConst131 = std::floor(0.027333f * fConst0 + 0.5f);
		iConst132 = static_cast<int>(std::min<float>(16384.0f, std::max<float>(0.0f, fConst120 - fConst131)));
		iConst133 = static_cast<int>(std::min<float>(2048.0f, std::max<float>(0.0f, fConst131 + -1.0f)));
		fConst134 = std::floor(0.192303f * fConst0 + 0.5f);
		fConst135 = fConst134 / fConst0;
		fConst136 = std::exp(-(4.934111f * fConst135));
		fConst137 = mydsp_faustpower2_f(fConst136);
		fConst138 = 1.0f - fConst104 * fConst137;
		fConst139 = 1.0f - fConst137;
		fConst140 = std::sqrt(std::max<float>(0.0f, mydsp_faustpower2_f(fConst138) / mydsp_faustpower2_f(fConst139) + -1.0f));
		fConst141 = fConst138 / fConst139;
		fConst142 = fConst136 * (fConst140 + (1.0f - fConst141));
		fConst143 = std::exp(-(3.1398888f * fConst135)) / fConst136 + -1.0f;
		fConst144 = fConst141 - fConst140;
		fConst145 = std::floor(0.029291f * fConst0 + 0.5f);
		iConst146 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, fConst134 - fConst145)));
		iConst147 = static_cast<int>(std::min<float>(2048.0f, std::max<float>(0.0f, fConst145 + -1.0f)));
		fConst148 = std::floor(0.210389f * fConst0 + 0.5f);
		fConst149 = fConst148 / fConst0;
		fConst150 = std::exp(-(4.934111f * fConst149));
		fConst151 = mydsp_faustpower2_f(fConst150);
		fConst152 = 1.0f - fConst104 * fConst151;
		fConst153 = 1.0f - fConst151;
		fConst154 = std::sqrt(std::max<float>(0.0f, mydsp_faustpower2_f(fConst152) / mydsp_faustpower2_f(fConst153) + -1.0f));
		fConst155 = fConst152 / fConst153;
		fConst156 = fConst150 * (fConst154 + (1.0f - fConst155));
		fConst157 = std::exp(-(3.1398888f * fConst149)) / fConst150 + -1.0f;
		fConst158 = fConst155 - fConst154;
		fConst159 = std::floor(0.024421f * fConst0 + 0.5f);
		iConst160 = static_cast<int>(std::min<float>(16384.0f, std::max<float>(0.0f, fConst148 - fConst159)));
		iConst161 = static_cast<int>(std::min<float>(2048.0f, std::max<float>(0.0f, fConst159 + -1.0f)));
		fConst162 = std::floor(0.125f * fConst0 + 0.5f);
		fConst163 = fConst162 / fConst0;
		fConst164 = std::exp(-(4.934111f * fConst163));
		fConst165 = mydsp_faustpower2_f(fConst164);
		fConst166 = 1.0f - fConst104 * fConst165;
		fConst167 = 1.0f - fConst165;
		fConst168 = std::sqrt(std::max<float>(0.0f, mydsp_faustpower2_f(fConst166) / mydsp_faustpower2_f(fConst167) + -1.0f));
		fConst169 = fConst166 / fConst167;
		fConst170 = fConst164 * (fConst168 + (1.0f - fConst169));
		fConst171 = std::exp(-(3.1398888f * fConst163)) / fConst164 + -1.0f;
		fConst172 = fConst169 - fConst168;
		fConst173 = std::floor(0.013458f * fConst0 + 0.5f);
		iConst174 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, fConst162 - fConst173)));
		iConst175 = static_cast<int>(std::min<float>(1024.0f, std::max<float>(0.0f, fConst173 + -1.0f)));
		fConst176 = std::floor(0.127837f * fConst0 + 0.5f);
		fConst177 = fConst176 / fConst0;
		fConst178 = std::exp(-(4.934111f * fConst177));
		fConst179 = mydsp_faustpower2_f(fConst178);
		fConst180 = 1.0f - fConst104 * fConst179;
		fConst181 = 1.0f - fConst179;
		fConst182 = std::sqrt(std::max<float>(0.0f, mydsp_faustpower2_f(fConst180) / mydsp_faustpower2_f(fConst181) + -1.0f));
		fConst183 = fConst180 / fConst181;
		fConst184 = fConst178 * (fConst182 + (1.0f - fConst183));
		fConst185 = std::exp(-(3.1398888f * fConst177)) / fConst178 + -1.0f;
		fConst186 = fConst183 - fConst182;
		fConst187 = std::floor(0.031604f * fConst0 + 0.5f);
		iConst188 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, fConst176 - fConst187)));
		iConst189 = static_cast<int>(std::min<float>(2048.0f, std::max<float>(0.0f, fConst187 + -1.0f)));
		fConst190 = std::floor(0.174713f * fConst0 + 0.5f);
		fConst191 = fConst190 / fConst0;
		fConst192 = std::exp(-(4.934111f * fConst191));
		fConst193 = mydsp_faustpower2_f(fConst192);
		fConst194 = 1.0f - fConst104 * fConst193;
		fConst195 = 1.0f - fConst193;
		fConst196 = std::sqrt(std::max<float>(0.0f, mydsp_faustpower2_f(fConst194) / mydsp_faustpower2_f(fConst195) + -1.0f));
		fConst197 = fConst194 / fConst195;
		fConst198 = fConst192 * (fConst196 + (1.0f - fConst197));
		fConst199 = std::exp(-(3.1398888f * fConst191)) / fConst192 + -1.0f;
		fConst200 = fConst197 - fConst196;
		fConst201 = std::floor(0.022904f * fConst0 + 0.5f);
		iConst202 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, fConst190 - fConst201)));
		iConst203 = static_cast<int>(std::min<float>(2048.0f, std::max<float>(0.0f, fConst201 + -1.0f)));
		fConst204 = std::floor(0.153129f * fConst0 + 0.5f);
		fConst205 = fConst204 / fConst0;
		fConst206 = std::exp(-(4.934111f * fConst205));
		fConst207 = mydsp_faustpower2_f(fConst206);
		fConst208 = 1.0f - fConst104 * fConst207;
		fConst209 = 1.0f - fConst207;
		fConst210 = std::sqrt(std::max<float>(0.0f, mydsp_faustpower2_f(fConst208) / mydsp_faustpower2_f(fConst209) + -1.0f));
		fConst211 = fConst208 / fConst209;
		fConst212 = fConst206 * (fConst210 + (1.0f - fConst211));
		fConst213 = std::exp(-(3.1398888f * fConst205)) / fConst206 + -1.0f;
		fConst214 = fConst211 - fConst210;
		fConst215 = std::floor(0.020346f * fConst0 + 0.5f);
		iConst216 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, fConst204 - fConst215)));
		iConst217 = static_cast<int>(std::min<float>(1024.0f, std::max<float>(0.0f, fConst215 + -1.0f)));
		fConst218 = 1.0f / std::tan(471.2389f / fConst0);
		fConst219 = 1.0f / (fConst218 + 1.0f);
		fConst220 = 1.0f - fConst218;
		fConst221 = 1.0f / std::tan(17506.525f / fConst0);
		fConst222 = 1.0f / (fConst221 + 1.0f);
		fConst223 = 1.0f - fConst221;
		fConst224 = std::round(0.035f * fConst0);
		fConst225 = std::max<float>(0.0f, fConst224);
		iConst226 = static_cast<int>(std::min<float>(std::round(0.05f * fConst0), fConst225));
		iConst227 = static_cast<int>(std::min<float>(fConst224, fConst225));
		iConst228 = static_cast<int>(std::min<float>(std::round(0.0425f * fConst0), fConst225));
		iConst229 = static_cast<int>(std::min<float>(std::round(0.0275f * fConst0), fConst225));
		iConst230 = static_cast<int>(std::min<float>(std::round(0.04625f * fConst0), fConst225));
		iConst231 = static_cast<int>(std::min<float>(std::round(0.03125f * fConst0), fConst225));
		iConst232 = static_cast<int>(std::min<float>(std::round(0.03875f * fConst0), fConst225));
		iConst233 = static_cast<int>(std::min<float>(std::round(0.02375f * fConst0), fConst225));
		iConst234 = static_cast<int>(std::min<float>(std::round(0.009f * fConst0), fConst225));
		iConst235 = static_cast<int>(std::min<float>(std::round(0.0195f * fConst0), fConst225));
		iConst236 = static_cast<int>(std::min<float>(std::round(0.01425f * fConst0), fConst225));
		iConst237 = static_cast<int>(std::min<float>(std::round(0.02475f * fConst0), fConst225));
		iConst238 = static_cast<int>(std::min<float>(std::round(0.011625f * fConst0), fConst225));
		iConst239 = static_cast<int>(std::min<float>(std::round(0.022125f * fConst0), fConst225));
		iConst240 = static_cast<int>(std::min<float>(std::round(0.016875f * fConst0), fConst225));
		iConst241 = static_cast<int>(std::min<float>(std::round(0.027375f * fConst0), fConst225));
		fConst242 = 0.01f * fConst0;
		iConst243 = static_cast<int>(std::min<float>(std::round(fConst242), fConst225));
		iConst244 = static_cast<int>(std::min<float>(std::round(0.0175f * fConst0), fConst225));
		iConst245 = static_cast<int>(std::min<float>(std::round(0.01375f * fConst0), fConst225));
		iConst246 = static_cast<int>(std::min<float>(std::round(0.02125f * fConst0), fConst225));
		iConst247 = static_cast<int>(std::min<float>(std::round(0.011875f * fConst0), fConst225));
		iConst248 = static_cast<int>(std::min<float>(std::round(0.019375f * fConst0), fConst225));
		iConst249 = static_cast<int>(std::min<float>(std::round(0.015625f * fConst0), fConst225));
		iConst250 = static_cast<int>(std::min<float>(std::round(0.023125f * fConst0), fConst225));
		fConst251 = std::round(0.08f * fConst0);
		iConst252 = static_cast<int>(std::min<float>(fConst251, std::max<float>(0.0f, std::round(0.0459048f * fConst0))));
		iConst253 = iConst252 + 1;
		iConst254 = static_cast<int>(std::min<float>(fConst251, std::max<float>(0.0f, std::round(0.0464048f * fConst0))));
		iConst255 = iConst254 + 1;
		iConst256 = static_cast<int>(std::min<float>(std::round(0.01475f * fConst0), fConst225));
		iConst257 = static_cast<int>(std::min<float>(fConst251, std::max<float>(0.0f, std::round(0.0469048f * fConst0))));
		iConst258 = iConst257 + 1;
		iConst259 = static_cast<int>(std::min<float>(std::round(0.017625f * fConst0), fConst225));
		iConst260 = static_cast<int>(std::min<float>(fConst251, std::max<float>(0.0f, std::round(0.0479048f * fConst0))));
		iConst261 = iConst260 + 1;
		iConst262 = static_cast<int>(std::min<float>(std::round(0.0205f * fConst0), fConst225));
		iConst263 = static_cast<int>(std::min<float>(fConst251, std::max<float>(0.0f, std::round(0.0489048f * fConst0))));
		iConst264 = iConst263 + 1;
		iConst265 = static_cast<int>(std::min<float>(std::round(0.023375f * fConst0), fConst225));
		iConst266 = static_cast<int>(std::min<float>(fConst251, std::max<float>(0.0f, std::round(0.0509048f * fConst0))));
		iConst267 = iConst266 + 1;
		iConst268 = static_cast<int>(std::min<float>(std::round(0.02625f * fConst0), fConst225));
		iConst269 = static_cast<int>(std::min<float>(fConst251, std::max<float>(0.0f, std::round(0.0519048f * fConst0))));
		iConst270 = iConst269 + 1;
		iConst271 = static_cast<int>(std::min<float>(std::round(0.029125f * fConst0), fConst225));
		iConst272 = static_cast<int>(std::min<float>(fConst251, std::max<float>(0.0f, std::round(0.0539048f * fConst0))));
		iConst273 = iConst272 + 1;
		iConst274 = static_cast<int>(std::min<float>(1024.0f, std::max<float>(0.0f, 0.015f * fConst0)));
		iConst275 = static_cast<int>(0.036666665f * fConst0);
		iConst276 = std::max<int>(0, iConst275 + 22);
		iConst277 = static_cast<int>(0.035306122f * fConst0);
		iConst278 = std::max<int>(0, iConst277 + 22);
		iConst279 = static_cast<int>(0.033809524f * fConst0);
		iConst280 = std::max<int>(0, iConst279 + 22);
		iConst281 = static_cast<int>(0.0322449f * fConst0);
		iConst282 = std::max<int>(0, iConst281 + 22);
		iConst283 = static_cast<int>(0.030748298f * fConst0);
		iConst284 = std::max<int>(0, iConst283 + 22);
		iConst285 = static_cast<int>(0.028956916f * fConst0);
		iConst286 = std::max<int>(0, iConst285 + 22);
		iConst287 = static_cast<int>(0.026938776f * fConst0);
		iConst288 = std::max<int>(0, iConst287 + 22);
		iConst289 = static_cast<int>(0.025306122f * fConst0);
		iConst290 = std::max<int>(0, iConst289 + 22);
		iConst291 = static_cast<int>(0.0126077095f * fConst0);
		iConst292 = std::min<int>(1024, std::max<int>(0, iConst291 + 22));
		iConst293 = static_cast<int>(fConst242);
		iConst294 = std::min<int>(1024, std::max<int>(0, iConst293 + 22));
		iConst295 = static_cast<int>(0.0077324263f * fConst0);
		iConst296 = std::min<int>(1024, std::max<int>(0, iConst295 + 22));
		iConst297 = static_cast<int>(0.0051020407f * fConst0);
		iConst298 = std::min<int>(1024, std::max<int>(0, iConst297 + 22));
		iConst299 = std::max<int>(0, iConst275 + -1);
		iConst300 = std::max<int>(0, iConst277 + -1);
		iConst301 = std::max<int>(0, iConst279 + -1);
		iConst302 = std::max<int>(0, iConst281 + -1);
		iConst303 = std::max<int>(0, iConst283 + -1);
		iConst304 = std::max<int>(0, iConst285 + -1);
		iConst305 = std::max<int>(0, iConst287 + -1);
		iConst306 = std::max<int>(0, iConst289 + -1);
		iConst307 = std::min<int>(1024, std::max<int>(0, iConst291 + -1));
		iConst308 = std::min<int>(1024, std::max<int>(0, iConst293 + -1));
		iConst309 = std::min<int>(1024, std::max<int>(0, iConst295 + -1));
		iConst310 = std::min<int>(1024, std::max<int>(0, iConst297 + -1));
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider0 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider1 = static_cast<FAUSTFLOAT>(1.0f);
		fCheckbox1 = static_cast<FAUSTFLOAT>(0.0f);
		fEntry0 = static_cast<FAUSTFLOAT>(3.0f);
		fCheckbox2 = static_cast<FAUSTFLOAT>(0.0f);
		fVslider0 = static_cast<FAUSTFLOAT>(0.3f);
		fVslider1 = static_cast<FAUSTFLOAT>(0.5f);
		fVslider2 = static_cast<FAUSTFLOAT>(0.75f);
		fHslider2 = static_cast<FAUSTFLOAT>(7.0f);
		fEntry1 = static_cast<FAUSTFLOAT>(2.0f);
		fHslider3 = static_cast<FAUSTFLOAT>(-6.0f);
		fCheckbox3 = static_cast<FAUSTFLOAT>(0.0f);
		fEntry2 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider4 = static_cast<FAUSTFLOAT>(-6.0f);
		fEntry3 = static_cast<FAUSTFLOAT>(0.0f);
		fEntry4 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider5 = static_cast<FAUSTFLOAT>(1.0f);
		fHslider6 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider7 = static_cast<FAUSTFLOAT>(-0.4f);
		fHslider8 = static_cast<FAUSTFLOAT>(2.3f);
		fHslider9 = static_cast<FAUSTFLOAT>(4.0f);
		fHslider10 = static_cast<FAUSTFLOAT>(1.0f);
		fHslider11 = static_cast<FAUSTFLOAT>(-3.0f);
		fHslider12 = static_cast<FAUSTFLOAT>(0.597f);
		fHslider13 = static_cast<FAUSTFLOAT>(1.0f);
		fEntry5 = static_cast<FAUSTFLOAT>(1.0f);
		fEntry6 = static_cast<FAUSTFLOAT>(0.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			fRec0[l0] = 0.0f;
		}
		for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
			fRec1[l1] = 0.0f;
		}
		for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
			fRec3[l2] = 0.0f;
		}
		for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
			fRec4[l3] = 0.0f;
		}
		for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
			fRec11[l4] = 0.0f;
		}
		for (int l5 = 0; l5 < 2; l5 = l5 + 1) {
			fRec13[l5] = 0.0f;
		}
		for (int l6 = 0; l6 < 2; l6 = l6 + 1) {
			fRec12[l6] = 0.0f;
		}
		for (int l7 = 0; l7 < 2; l7 = l7 + 1) {
			fRec14[l7] = 0.0f;
		}
		for (int l8 = 0; l8 < 2; l8 = l8 + 1) {
			fRec18[l8] = 0.0f;
		}
		for (int l9 = 0; l9 < 2; l9 = l9 + 1) {
			fRec20[l9] = 0.0f;
		}
		for (int l10 = 0; l10 < 2; l10 = l10 + 1) {
			fVec0[l10] = 0.0f;
		}
		for (int l11 = 0; l11 < 2; l11 = l11 + 1) {
			fRec23[l11] = 0.0f;
		}
		for (int l12 = 0; l12 < 2; l12 = l12 + 1) {
			fRec22[l12] = 0.0f;
		}
		for (int l13 = 0; l13 < 3; l13 = l13 + 1) {
			fRec21[l13] = 0.0f;
		}
		for (int l14 = 0; l14 < 2; l14 = l14 + 1) {
			fRec19[l14] = 0.0f;
		}
		for (int l15 = 0; l15 < 3; l15 = l15 + 1) {
			fRec17[l15] = 0.0f;
		}
		for (int l16 = 0; l16 < 2; l16 = l16 + 1) {
			fRec16[l16] = 0.0f;
		}
		for (int l17 = 0; l17 < 2; l17 = l17 + 1) {
			fRec26[l17] = 0.0f;
		}
		for (int l18 = 0; l18 < 3; l18 = l18 + 1) {
			fRec25[l18] = 0.0f;
		}
		for (int l19 = 0; l19 < 2; l19 = l19 + 1) {
			fRec24[l19] = 0.0f;
		}
		for (int l20 = 0; l20 < 2; l20 = l20 + 1) {
			fRec29[l20] = 0.0f;
		}
		for (int l21 = 0; l21 < 3; l21 = l21 + 1) {
			fRec28[l21] = 0.0f;
		}
		for (int l22 = 0; l22 < 2; l22 = l22 + 1) {
			fRec27[l22] = 0.0f;
		}
		for (int l23 = 0; l23 < 2; l23 = l23 + 1) {
			fRec32[l23] = 0.0f;
		}
		for (int l24 = 0; l24 < 3; l24 = l24 + 1) {
			fRec31[l24] = 0.0f;
		}
		for (int l25 = 0; l25 < 2; l25 = l25 + 1) {
			fRec30[l25] = 0.0f;
		}
		for (int l26 = 0; l26 < 2; l26 = l26 + 1) {
			fRec35[l26] = 0.0f;
		}
		for (int l27 = 0; l27 < 3; l27 = l27 + 1) {
			fRec34[l27] = 0.0f;
		}
		for (int l28 = 0; l28 < 2; l28 = l28 + 1) {
			fRec33[l28] = 0.0f;
		}
		for (int l29 = 0; l29 < 2; l29 = l29 + 1) {
			fRec38[l29] = 0.0f;
		}
		for (int l30 = 0; l30 < 3; l30 = l30 + 1) {
			fRec37[l30] = 0.0f;
		}
		for (int l31 = 0; l31 < 2; l31 = l31 + 1) {
			fRec36[l31] = 0.0f;
		}
		for (int l32 = 0; l32 < 2; l32 = l32 + 1) {
			fRec15[l32] = 0.0f;
		}
		for (int l33 = 0; l33 < 2; l33 = l33 + 1) {
			fRec42[l33] = 0.0f;
		}
		for (int l34 = 0; l34 < 2; l34 = l34 + 1) {
			fVec1[l34] = 0.0f;
		}
		for (int l35 = 0; l35 < 2; l35 = l35 + 1) {
			fRec43[l35] = 0.0f;
		}
		for (int l36 = 0; l36 < 3; l36 = l36 + 1) {
			fRec41[l36] = 0.0f;
		}
		for (int l37 = 0; l37 < 2; l37 = l37 + 1) {
			fRec40[l37] = 0.0f;
		}
		for (int l38 = 0; l38 < 2; l38 = l38 + 1) {
			fRec46[l38] = 0.0f;
		}
		for (int l39 = 0; l39 < 3; l39 = l39 + 1) {
			fRec45[l39] = 0.0f;
		}
		for (int l40 = 0; l40 < 2; l40 = l40 + 1) {
			fRec44[l40] = 0.0f;
		}
		for (int l41 = 0; l41 < 2; l41 = l41 + 1) {
			fRec49[l41] = 0.0f;
		}
		for (int l42 = 0; l42 < 3; l42 = l42 + 1) {
			fRec48[l42] = 0.0f;
		}
		for (int l43 = 0; l43 < 2; l43 = l43 + 1) {
			fRec47[l43] = 0.0f;
		}
		for (int l44 = 0; l44 < 2; l44 = l44 + 1) {
			fRec52[l44] = 0.0f;
		}
		for (int l45 = 0; l45 < 3; l45 = l45 + 1) {
			fRec51[l45] = 0.0f;
		}
		for (int l46 = 0; l46 < 2; l46 = l46 + 1) {
			fRec50[l46] = 0.0f;
		}
		for (int l47 = 0; l47 < 2; l47 = l47 + 1) {
			fRec55[l47] = 0.0f;
		}
		for (int l48 = 0; l48 < 3; l48 = l48 + 1) {
			fRec54[l48] = 0.0f;
		}
		for (int l49 = 0; l49 < 2; l49 = l49 + 1) {
			fRec53[l49] = 0.0f;
		}
		for (int l50 = 0; l50 < 2; l50 = l50 + 1) {
			fRec58[l50] = 0.0f;
		}
		for (int l51 = 0; l51 < 3; l51 = l51 + 1) {
			fRec57[l51] = 0.0f;
		}
		for (int l52 = 0; l52 < 2; l52 = l52 + 1) {
			fRec56[l52] = 0.0f;
		}
		for (int l53 = 0; l53 < 2; l53 = l53 + 1) {
			fRec39[l53] = 0.0f;
		}
		for (int l54 = 0; l54 < 2; l54 = l54 + 1) {
			fRec61[l54] = 0.0f;
		}
		for (int l55 = 0; l55 < 2; l55 = l55 + 1) {
			fVec2[l55] = 0.0f;
		}
		for (int l56 = 0; l56 < 2; l56 = l56 + 1) {
			fRec62[l56] = 0.0f;
		}
		for (int l57 = 0; l57 < 3; l57 = l57 + 1) {
			fRec60[l57] = 0.0f;
		}
		for (int l58 = 0; l58 < 2; l58 = l58 + 1) {
			fRec59[l58] = 0.0f;
		}
		for (int l59 = 0; l59 < 2; l59 = l59 + 1) {
			fRec65[l59] = 0.0f;
		}
		for (int l60 = 0; l60 < 3; l60 = l60 + 1) {
			fRec64[l60] = 0.0f;
		}
		for (int l61 = 0; l61 < 2; l61 = l61 + 1) {
			fRec63[l61] = 0.0f;
		}
		for (int l62 = 0; l62 < 2; l62 = l62 + 1) {
			fRec68[l62] = 0.0f;
		}
		for (int l63 = 0; l63 < 3; l63 = l63 + 1) {
			fRec67[l63] = 0.0f;
		}
		for (int l64 = 0; l64 < 2; l64 = l64 + 1) {
			fRec66[l64] = 0.0f;
		}
		for (int l65 = 0; l65 < 2; l65 = l65 + 1) {
			fRec71[l65] = 0.0f;
		}
		for (int l66 = 0; l66 < 3; l66 = l66 + 1) {
			fRec70[l66] = 0.0f;
		}
		for (int l67 = 0; l67 < 2; l67 = l67 + 1) {
			fRec69[l67] = 0.0f;
		}
		for (int l68 = 0; l68 < 2; l68 = l68 + 1) {
			fRec74[l68] = 0.0f;
		}
		for (int l69 = 0; l69 < 3; l69 = l69 + 1) {
			fRec73[l69] = 0.0f;
		}
		for (int l70 = 0; l70 < 2; l70 = l70 + 1) {
			fRec72[l70] = 0.0f;
		}
		for (int l71 = 0; l71 < 2; l71 = l71 + 1) {
			fRec77[l71] = 0.0f;
		}
		for (int l72 = 0; l72 < 3; l72 = l72 + 1) {
			fRec76[l72] = 0.0f;
		}
		for (int l73 = 0; l73 < 2; l73 = l73 + 1) {
			fRec75[l73] = 0.0f;
		}
		for (int l74 = 0; l74 < 2; l74 = l74 + 1) {
			fVec3[l74] = 0.0f;
		}
		for (int l75 = 0; l75 < 2; l75 = l75 + 1) {
			fRec10[l75] = 0.0f;
		}
		for (int l76 = 0; l76 < 3; l76 = l76 + 1) {
			fRec9[l76] = 0.0f;
		}
		for (int l77 = 0; l77 < 2; l77 = l77 + 1) {
			fRec79[l77] = 0.0f;
		}
		for (int l78 = 0; l78 < 3; l78 = l78 + 1) {
			fRec78[l78] = 0.0f;
		}
		for (int l79 = 0; l79 < 3; l79 = l79 + 1) {
			fRec8[l79] = 0.0f;
		}
		for (int l80 = 0; l80 < 2; l80 = l80 + 1) {
			fVec4[l80] = 0.0f;
		}
		for (int l81 = 0; l81 < 2; l81 = l81 + 1) {
			fRec7[l81] = 0.0f;
		}
		for (int l82 = 0; l82 < 3; l82 = l82 + 1) {
			fRec6[l82] = 0.0f;
		}
		for (int l83 = 0; l83 < 2; l83 = l83 + 1) {
			fRec81[l83] = 0.0f;
		}
		for (int l84 = 0; l84 < 3; l84 = l84 + 1) {
			fRec80[l84] = 0.0f;
		}
		for (int l85 = 0; l85 < 4; l85 = l85 + 1) {
			fRec5[l85] = 0.0f;
		}
		for (int l86 = 0; l86 < 4; l86 = l86 + 1) {
			fRec82[l86] = 0.0f;
		}
		for (int l87 = 0; l87 < 4; l87 = l87 + 1) {
			fRec83[l87] = 0.0f;
		}
		for (int l88 = 0; l88 < 4; l88 = l88 + 1) {
			fRec84[l88] = 0.0f;
		}
		for (int l89 = 0; l89 < 2; l89 = l89 + 1) {
			fRec88[l89] = 0.0f;
		}
		for (int l90 = 0; l90 < 2; l90 = l90 + 1) {
			fRec89[l90] = 0.0f;
		}
		for (int l91 = 0; l91 < 3; l91 = l91 + 1) {
			fRec91[l91] = 0.0f;
		}
		for (int l92 = 0; l92 < 3; l92 = l92 + 1) {
			fRec90[l92] = 0.0f;
		}
		for (int l93 = 0; l93 < 2; l93 = l93 + 1) {
			fRec92[l93] = 0.0f;
		}
		for (int l94 = 0; l94 < 2; l94 = l94 + 1) {
			fRec93[l94] = 0.0f;
		}
		for (int l95 = 0; l95 < 2; l95 = l95 + 1) {
			fRec94[l95] = 0.0f;
		}
		for (int l96 = 0; l96 < 2; l96 = l96 + 1) {
			fRec95[l96] = 0.0f;
		}
		for (int l97 = 0; l97 < 2; l97 = l97 + 1) {
			fVec5[l97] = 0.0f;
		}
		for (int l98 = 0; l98 < 2; l98 = l98 + 1) {
			fRec87[l98] = 0.0f;
		}
		for (int l99 = 0; l99 < 2; l99 = l99 + 1) {
			fRec86[l99] = 0.0f;
		}
		for (int l100 = 0; l100 < 2; l100 = l100 + 1) {
			fRec85[l100] = 0.0f;
		}
		for (int l101 = 0; l101 < 2; l101 = l101 + 1) {
			fRec96[l101] = 0.0f;
		}
		for (int l102 = 0; l102 < 2; l102 = l102 + 1) {
			fVec6[l102] = 0.0f;
		}
		for (int l103 = 0; l103 < 2; l103 = l103 + 1) {
			fRec2[l103] = 0.0f;
		}
		for (int l104 = 0; l104 < 2; l104 = l104 + 1) {
			fRec97[l104] = 0.0f;
		}
		for (int l105 = 0; l105 < 2; l105 = l105 + 1) {
			fRec98[l105] = 0.0f;
		}
		for (int l106 = 0; l106 < 2; l106 = l106 + 1) {
			fRec110[l106] = 0.0f;
		}
		for (int l107 = 0; l107 < 2; l107 = l107 + 1) {
			fRec109[l107] = 0.0f;
		}
		IOTA0 = 0;
		for (int l108 = 0; l108 < 32768; l108 = l108 + 1) {
			fVec7[l108] = 0.0f;
		}
		for (int l109 = 0; l109 < 8192; l109 = l109 + 1) {
			fVec8[l109] = 0.0f;
		}
		for (int l110 = 0; l110 < 2048; l110 = l110 + 1) {
			fVec9[l110] = 0.0f;
		}
		for (int l111 = 0; l111 < 2; l111 = l111 + 1) {
			fRec107[l111] = 0.0f;
		}
		for (int l112 = 0; l112 < 2; l112 = l112 + 1) {
			fRec114[l112] = 0.0f;
		}
		for (int l113 = 0; l113 < 2; l113 = l113 + 1) {
			fRec113[l113] = 0.0f;
		}
		for (int l114 = 0; l114 < 32768; l114 = l114 + 1) {
			fVec10[l114] = 0.0f;
		}
		for (int l115 = 0; l115 < 4096; l115 = l115 + 1) {
			fVec11[l115] = 0.0f;
		}
		for (int l116 = 0; l116 < 2; l116 = l116 + 1) {
			fRec111[l116] = 0.0f;
		}
		for (int l117 = 0; l117 < 2; l117 = l117 + 1) {
			fRec118[l117] = 0.0f;
		}
		for (int l118 = 0; l118 < 2; l118 = l118 + 1) {
			fRec117[l118] = 0.0f;
		}
		for (int l119 = 0; l119 < 16384; l119 = l119 + 1) {
			fVec12[l119] = 0.0f;
		}
		for (int l120 = 0; l120 < 4096; l120 = l120 + 1) {
			fVec13[l120] = 0.0f;
		}
		for (int l121 = 0; l121 < 2; l121 = l121 + 1) {
			fRec115[l121] = 0.0f;
		}
		for (int l122 = 0; l122 < 2; l122 = l122 + 1) {
			fRec122[l122] = 0.0f;
		}
		for (int l123 = 0; l123 < 2; l123 = l123 + 1) {
			fRec121[l123] = 0.0f;
		}
		for (int l124 = 0; l124 < 32768; l124 = l124 + 1) {
			fVec14[l124] = 0.0f;
		}
		for (int l125 = 0; l125 < 4096; l125 = l125 + 1) {
			fVec15[l125] = 0.0f;
		}
		for (int l126 = 0; l126 < 2; l126 = l126 + 1) {
			fRec119[l126] = 0.0f;
		}
		for (int l127 = 0; l127 < 2; l127 = l127 + 1) {
			fRec126[l127] = 0.0f;
		}
		for (int l128 = 0; l128 < 2; l128 = l128 + 1) {
			fRec125[l128] = 0.0f;
		}
		for (int l129 = 0; l129 < 16384; l129 = l129 + 1) {
			fVec16[l129] = 0.0f;
		}
		for (int l130 = 0; l130 < 2048; l130 = l130 + 1) {
			fVec17[l130] = 0.0f;
		}
		for (int l131 = 0; l131 < 2; l131 = l131 + 1) {
			fRec123[l131] = 0.0f;
		}
		for (int l132 = 0; l132 < 2; l132 = l132 + 1) {
			fRec130[l132] = 0.0f;
		}
		for (int l133 = 0; l133 < 2; l133 = l133 + 1) {
			fRec129[l133] = 0.0f;
		}
		for (int l134 = 0; l134 < 16384; l134 = l134 + 1) {
			fVec18[l134] = 0.0f;
		}
		for (int l135 = 0; l135 < 4096; l135 = l135 + 1) {
			fVec19[l135] = 0.0f;
		}
		for (int l136 = 0; l136 < 2; l136 = l136 + 1) {
			fRec127[l136] = 0.0f;
		}
		for (int l137 = 0; l137 < 2; l137 = l137 + 1) {
			fRec134[l137] = 0.0f;
		}
		for (int l138 = 0; l138 < 2; l138 = l138 + 1) {
			fRec133[l138] = 0.0f;
		}
		for (int l139 = 0; l139 < 16384; l139 = l139 + 1) {
			fVec20[l139] = 0.0f;
		}
		for (int l140 = 0; l140 < 4096; l140 = l140 + 1) {
			fVec21[l140] = 0.0f;
		}
		for (int l141 = 0; l141 < 2; l141 = l141 + 1) {
			fRec131[l141] = 0.0f;
		}
		for (int l142 = 0; l142 < 2; l142 = l142 + 1) {
			fRec138[l142] = 0.0f;
		}
		for (int l143 = 0; l143 < 2; l143 = l143 + 1) {
			fRec137[l143] = 0.0f;
		}
		for (int l144 = 0; l144 < 16384; l144 = l144 + 1) {
			fVec22[l144] = 0.0f;
		}
		for (int l145 = 0; l145 < 2048; l145 = l145 + 1) {
			fVec23[l145] = 0.0f;
		}
		for (int l146 = 0; l146 < 2; l146 = l146 + 1) {
			fRec135[l146] = 0.0f;
		}
		for (int l147 = 0; l147 < 3; l147 = l147 + 1) {
			fRec99[l147] = 0.0f;
		}
		for (int l148 = 0; l148 < 3; l148 = l148 + 1) {
			fRec100[l148] = 0.0f;
		}
		for (int l149 = 0; l149 < 3; l149 = l149 + 1) {
			fRec101[l149] = 0.0f;
		}
		for (int l150 = 0; l150 < 3; l150 = l150 + 1) {
			fRec102[l150] = 0.0f;
		}
		for (int l151 = 0; l151 < 3; l151 = l151 + 1) {
			fRec103[l151] = 0.0f;
		}
		for (int l152 = 0; l152 < 3; l152 = l152 + 1) {
			fRec104[l152] = 0.0f;
		}
		for (int l153 = 0; l153 < 3; l153 = l153 + 1) {
			fRec105[l153] = 0.0f;
		}
		for (int l154 = 0; l154 < 3; l154 = l154 + 1) {
			fRec106[l154] = 0.0f;
		}
		for (int l155 = 0; l155 < 2048; l155 = l155 + 1) {
			fVec24[l155] = 0.0f;
		}
		for (int l156 = 0; l156 < 4096; l156 = l156 + 1) {
			fVec25[l156] = 0.0f;
		}
		for (int l157 = 0; l157 < 4096; l157 = l157 + 1) {
			fVec26[l157] = 0.0f;
		}
		for (int l158 = 0; l158 < 8192; l158 = l158 + 1) {
			fVec27[l158] = 0.0f;
		}
		for (int l159 = 0; l159 < 4096; l159 = l159 + 1) {
			fVec28[l159] = 0.0f;
		}
		for (int l160 = 0; l160 < 8192; l160 = l160 + 1) {
			fVec29[l160] = 0.0f;
		}
		for (int l161 = 0; l161 < 4096; l161 = l161 + 1) {
			fVec30[l161] = 0.0f;
		}
		for (int l162 = 0; l162 < 8192; l162 = l162 + 1) {
			fVec31[l162] = 0.0f;
		}
		for (int l163 = 0; l163 < 2048; l163 = l163 + 1) {
			fVec32[l163] = 0.0f;
		}
		for (int l164 = 0; l164 < 4096; l164 = l164 + 1) {
			fVec33[l164] = 0.0f;
		}
		for (int l165 = 0; l165 < 4096; l165 = l165 + 1) {
			fVec34[l165] = 0.0f;
		}
		for (int l166 = 0; l166 < 4096; l166 = l166 + 1) {
			fVec35[l166] = 0.0f;
		}
		for (int l167 = 0; l167 < 4096; l167 = l167 + 1) {
			fVec36[l167] = 0.0f;
		}
		for (int l168 = 0; l168 < 4096; l168 = l168 + 1) {
			fVec37[l168] = 0.0f;
		}
		for (int l169 = 0; l169 < 4096; l169 = l169 + 1) {
			fVec38[l169] = 0.0f;
		}
		for (int l170 = 0; l170 < 8192; l170 = l170 + 1) {
			fVec39[l170] = 0.0f;
		}
		for (int l171 = 0; l171 < 2048; l171 = l171 + 1) {
			fVec40[l171] = 0.0f;
		}
		for (int l172 = 0; l172 < 16384; l172 = l172 + 1) {
			fVec41[l172] = 0.0f;
		}
		for (int l173 = 0; l173 < 2; l173 = l173 + 1) {
			fRec148[l173] = 0.0f;
		}
		for (int l174 = 0; l174 < 2; l174 = l174 + 1) {
			fRec140[l174] = 0.0f;
		}
		for (int l175 = 0; l175 < 4096; l175 = l175 + 1) {
			fVec42[l175] = 0.0f;
		}
		for (int l176 = 0; l176 < 16384; l176 = l176 + 1) {
			fVec43[l176] = 0.0f;
		}
		for (int l177 = 0; l177 < 2; l177 = l177 + 1) {
			fRec149[l177] = 0.0f;
		}
		for (int l178 = 0; l178 < 2; l178 = l178 + 1) {
			fRec141[l178] = 0.0f;
		}
		for (int l179 = 0; l179 < 4096; l179 = l179 + 1) {
			fVec44[l179] = 0.0f;
		}
		for (int l180 = 0; l180 < 16384; l180 = l180 + 1) {
			fVec45[l180] = 0.0f;
		}
		for (int l181 = 0; l181 < 2; l181 = l181 + 1) {
			fRec150[l181] = 0.0f;
		}
		for (int l182 = 0; l182 < 2; l182 = l182 + 1) {
			fRec142[l182] = 0.0f;
		}
		for (int l183 = 0; l183 < 4096; l183 = l183 + 1) {
			fVec46[l183] = 0.0f;
		}
		for (int l184 = 0; l184 < 16384; l184 = l184 + 1) {
			fVec47[l184] = 0.0f;
		}
		for (int l185 = 0; l185 < 2; l185 = l185 + 1) {
			fRec151[l185] = 0.0f;
		}
		for (int l186 = 0; l186 < 2; l186 = l186 + 1) {
			fRec143[l186] = 0.0f;
		}
		for (int l187 = 0; l187 < 4096; l187 = l187 + 1) {
			fVec48[l187] = 0.0f;
		}
		for (int l188 = 0; l188 < 16384; l188 = l188 + 1) {
			fVec49[l188] = 0.0f;
		}
		for (int l189 = 0; l189 < 2; l189 = l189 + 1) {
			fRec152[l189] = 0.0f;
		}
		for (int l190 = 0; l190 < 2; l190 = l190 + 1) {
			fRec144[l190] = 0.0f;
		}
		for (int l191 = 0; l191 < 8192; l191 = l191 + 1) {
			fVec50[l191] = 0.0f;
		}
		for (int l192 = 0; l192 < 16384; l192 = l192 + 1) {
			fVec51[l192] = 0.0f;
		}
		for (int l193 = 0; l193 < 2; l193 = l193 + 1) {
			fRec153[l193] = 0.0f;
		}
		for (int l194 = 0; l194 < 2; l194 = l194 + 1) {
			fRec145[l194] = 0.0f;
		}
		for (int l195 = 0; l195 < 8192; l195 = l195 + 1) {
			fVec52[l195] = 0.0f;
		}
		for (int l196 = 0; l196 < 16384; l196 = l196 + 1) {
			fVec53[l196] = 0.0f;
		}
		for (int l197 = 0; l197 < 2; l197 = l197 + 1) {
			fRec154[l197] = 0.0f;
		}
		for (int l198 = 0; l198 < 2; l198 = l198 + 1) {
			fRec146[l198] = 0.0f;
		}
		for (int l199 = 0; l199 < 8192; l199 = l199 + 1) {
			fVec54[l199] = 0.0f;
		}
		for (int l200 = 0; l200 < 16384; l200 = l200 + 1) {
			fVec55[l200] = 0.0f;
		}
		for (int l201 = 0; l201 < 2; l201 = l201 + 1) {
			fRec155[l201] = 0.0f;
		}
		for (int l202 = 0; l202 < 2; l202 = l202 + 1) {
			fRec147[l202] = 0.0f;
		}
		for (int l203 = 0; l203 < 2; l203 = l203 + 1) {
			fVec56[l203] = 0.0f;
		}
		for (int l204 = 0; l204 < 2; l204 = l204 + 1) {
			fRec139[l204] = 0.0f;
		}
		for (int l205 = 0; l205 < 2048; l205 = l205 + 1) {
			fRec165[l205] = 0.0f;
		}
		for (int l206 = 0; l206 < 2; l206 = l206 + 1) {
			fRec166[l206] = 0.0f;
		}
		for (int l207 = 0; l207 < 8192; l207 = l207 + 1) {
			fVec57[l207] = 0.0f;
		}
		for (int l208 = 0; l208 < 2; l208 = l208 + 1) {
			fRec164[l208] = 0.0f;
		}
		for (int l209 = 0; l209 < 2; l209 = l209 + 1) {
			fRec168[l209] = 0.0f;
		}
		for (int l210 = 0; l210 < 8192; l210 = l210 + 1) {
			fVec58[l210] = 0.0f;
		}
		for (int l211 = 0; l211 < 2; l211 = l211 + 1) {
			fRec167[l211] = 0.0f;
		}
		for (int l212 = 0; l212 < 2; l212 = l212 + 1) {
			fRec170[l212] = 0.0f;
		}
		for (int l213 = 0; l213 < 8192; l213 = l213 + 1) {
			fVec59[l213] = 0.0f;
		}
		for (int l214 = 0; l214 < 2; l214 = l214 + 1) {
			fRec169[l214] = 0.0f;
		}
		for (int l215 = 0; l215 < 2; l215 = l215 + 1) {
			fRec172[l215] = 0.0f;
		}
		for (int l216 = 0; l216 < 8192; l216 = l216 + 1) {
			fVec60[l216] = 0.0f;
		}
		for (int l217 = 0; l217 < 2; l217 = l217 + 1) {
			fRec171[l217] = 0.0f;
		}
		for (int l218 = 0; l218 < 2; l218 = l218 + 1) {
			fRec174[l218] = 0.0f;
		}
		for (int l219 = 0; l219 < 8192; l219 = l219 + 1) {
			fVec61[l219] = 0.0f;
		}
		for (int l220 = 0; l220 < 2; l220 = l220 + 1) {
			fRec173[l220] = 0.0f;
		}
		for (int l221 = 0; l221 < 2; l221 = l221 + 1) {
			fRec176[l221] = 0.0f;
		}
		for (int l222 = 0; l222 < 8192; l222 = l222 + 1) {
			fVec62[l222] = 0.0f;
		}
		for (int l223 = 0; l223 < 2; l223 = l223 + 1) {
			fRec175[l223] = 0.0f;
		}
		for (int l224 = 0; l224 < 2; l224 = l224 + 1) {
			fRec178[l224] = 0.0f;
		}
		for (int l225 = 0; l225 < 8192; l225 = l225 + 1) {
			fVec63[l225] = 0.0f;
		}
		for (int l226 = 0; l226 < 2; l226 = l226 + 1) {
			fRec177[l226] = 0.0f;
		}
		for (int l227 = 0; l227 < 2; l227 = l227 + 1) {
			fRec180[l227] = 0.0f;
		}
		for (int l228 = 0; l228 < 8192; l228 = l228 + 1) {
			fVec64[l228] = 0.0f;
		}
		for (int l229 = 0; l229 < 2; l229 = l229 + 1) {
			fRec179[l229] = 0.0f;
		}
		for (int l230 = 0; l230 < 2048; l230 = l230 + 1) {
			fVec65[l230] = 0.0f;
		}
		for (int l231 = 0; l231 < 2; l231 = l231 + 1) {
			fRec162[l231] = 0.0f;
		}
		for (int l232 = 0; l232 < 2048; l232 = l232 + 1) {
			fVec66[l232] = 0.0f;
		}
		for (int l233 = 0; l233 < 2; l233 = l233 + 1) {
			fRec160[l233] = 0.0f;
		}
		for (int l234 = 0; l234 < 2048; l234 = l234 + 1) {
			fVec67[l234] = 0.0f;
		}
		for (int l235 = 0; l235 < 2; l235 = l235 + 1) {
			fRec158[l235] = 0.0f;
		}
		for (int l236 = 0; l236 < 1024; l236 = l236 + 1) {
			fVec68[l236] = 0.0f;
		}
		for (int l237 = 0; l237 < 2; l237 = l237 + 1) {
			fRec156[l237] = 0.0f;
		}
		for (int l238 = 0; l238 < 2; l238 = l238 + 1) {
			fRec190[l238] = 0.0f;
		}
		for (int l239 = 0; l239 < 8192; l239 = l239 + 1) {
			fVec69[l239] = 0.0f;
		}
		for (int l240 = 0; l240 < 2; l240 = l240 + 1) {
			fRec189[l240] = 0.0f;
		}
		for (int l241 = 0; l241 < 2; l241 = l241 + 1) {
			fRec192[l241] = 0.0f;
		}
		for (int l242 = 0; l242 < 8192; l242 = l242 + 1) {
			fVec70[l242] = 0.0f;
		}
		for (int l243 = 0; l243 < 2; l243 = l243 + 1) {
			fRec191[l243] = 0.0f;
		}
		for (int l244 = 0; l244 < 2; l244 = l244 + 1) {
			fRec194[l244] = 0.0f;
		}
		for (int l245 = 0; l245 < 8192; l245 = l245 + 1) {
			fVec71[l245] = 0.0f;
		}
		for (int l246 = 0; l246 < 2; l246 = l246 + 1) {
			fRec193[l246] = 0.0f;
		}
		for (int l247 = 0; l247 < 2; l247 = l247 + 1) {
			fRec196[l247] = 0.0f;
		}
		for (int l248 = 0; l248 < 8192; l248 = l248 + 1) {
			fVec72[l248] = 0.0f;
		}
		for (int l249 = 0; l249 < 2; l249 = l249 + 1) {
			fRec195[l249] = 0.0f;
		}
		for (int l250 = 0; l250 < 2; l250 = l250 + 1) {
			fRec198[l250] = 0.0f;
		}
		for (int l251 = 0; l251 < 8192; l251 = l251 + 1) {
			fVec73[l251] = 0.0f;
		}
		for (int l252 = 0; l252 < 2; l252 = l252 + 1) {
			fRec197[l252] = 0.0f;
		}
		for (int l253 = 0; l253 < 2; l253 = l253 + 1) {
			fRec200[l253] = 0.0f;
		}
		for (int l254 = 0; l254 < 8192; l254 = l254 + 1) {
			fVec74[l254] = 0.0f;
		}
		for (int l255 = 0; l255 < 2; l255 = l255 + 1) {
			fRec199[l255] = 0.0f;
		}
		for (int l256 = 0; l256 < 2; l256 = l256 + 1) {
			fRec202[l256] = 0.0f;
		}
		for (int l257 = 0; l257 < 8192; l257 = l257 + 1) {
			fVec75[l257] = 0.0f;
		}
		for (int l258 = 0; l258 < 2; l258 = l258 + 1) {
			fRec201[l258] = 0.0f;
		}
		for (int l259 = 0; l259 < 2; l259 = l259 + 1) {
			fRec204[l259] = 0.0f;
		}
		for (int l260 = 0; l260 < 8192; l260 = l260 + 1) {
			fVec76[l260] = 0.0f;
		}
		for (int l261 = 0; l261 < 2; l261 = l261 + 1) {
			fRec203[l261] = 0.0f;
		}
		for (int l262 = 0; l262 < 2048; l262 = l262 + 1) {
			fVec77[l262] = 0.0f;
		}
		for (int l263 = 0; l263 < 2; l263 = l263 + 1) {
			fRec187[l263] = 0.0f;
		}
		for (int l264 = 0; l264 < 2048; l264 = l264 + 1) {
			fVec78[l264] = 0.0f;
		}
		for (int l265 = 0; l265 < 2; l265 = l265 + 1) {
			fRec185[l265] = 0.0f;
		}
		for (int l266 = 0; l266 < 2048; l266 = l266 + 1) {
			fVec79[l266] = 0.0f;
		}
		for (int l267 = 0; l267 < 2; l267 = l267 + 1) {
			fRec183[l267] = 0.0f;
		}
		for (int l268 = 0; l268 < 1024; l268 = l268 + 1) {
			fVec80[l268] = 0.0f;
		}
		for (int l269 = 0; l269 < 2; l269 = l269 + 1) {
			fRec181[l269] = 0.0f;
		}
		for (int l270 = 0; l270 < 1024; l270 = l270 + 1) {
			fVec81[l270] = 0.0f;
		}
		for (int l271 = 0; l271 < 1024; l271 = l271 + 1) {
			fVec82[l271] = 0.0f;
		}
		for (int l272 = 0; l272 < 1024; l272 = l272 + 1) {
			fVec83[l272] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp(*this);
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("guitar tube amp sim 100% FAUST");
		ui_interface->openHorizontalBox("4 Tonestack");
		ui_interface->openHorizontalBox("4 Tonestack");
		ui_interface->openHorizontalBox("EQ");
		ui_interface->declare(&fVslider1, "1", "");
		ui_interface->declare(&fVslider1, "style", "knob");
		ui_interface->addVerticalSlider("Bass", &fVslider1, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->declare(&fVslider0, "2", "");
		ui_interface->declare(&fVslider0, "style", "knob");
		ui_interface->addVerticalSlider("Middle", &fVslider0, FAUSTFLOAT(0.3f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->declare(&fVslider2, "3", "");
		ui_interface->declare(&fVslider2, "style", "knob");
		ui_interface->addVerticalSlider("Treble", &fVslider2, FAUSTFLOAT(0.75f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("tonestack type");
		ui_interface->declare(&fEntry0, "style", "menu{'Mesa Boogie':0;'JCM800':1;'AC30':2;'Fender Hot Rod':3}");
		ui_interface->addNumEntry("Model", &fEntry0, FAUSTFLOAT(3.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(3.0f), FAUSTFLOAT(1.0f));
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->addCheckButton("bypass", &fCheckbox2);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("5 Power Amp");
		ui_interface->declare(&fHslider10, "style", "knob");
		ui_interface->addHorizontalSlider("Curve k", &fHslider10, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.1f), FAUSTFLOAT(4.0f), FAUSTFLOAT(0.001f));
		ui_interface->declare(&fHslider9, "style", "knob");
		ui_interface->addHorizontalSlider("Drive gain", &fHslider9, FAUSTFLOAT(4.0f), FAUSTFLOAT(-1e+01f), FAUSTFLOAT(1e+01f), FAUSTFLOAT(0.001f));
		ui_interface->declare(&fHslider11, "style", "knob");
		ui_interface->addHorizontalSlider("Level", &fHslider11, FAUSTFLOAT(-3.0f), FAUSTFLOAT(-24.0f), FAUSTFLOAT(24.0f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider8, "name", "MV");
		ui_interface->declare(&fHslider8, "style", "knob");
		ui_interface->addHorizontalSlider("Master Volume", &fHslider8, FAUSTFLOAT(2.3f), FAUSTFLOAT(0.0f), FAUSTFLOAT(4.0f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fHslider7, "name", "Level");
		ui_interface->declare(&fHslider7, "style", "knob");
		ui_interface->addHorizontalSlider("Negative gain", &fHslider7, FAUSTFLOAT(-0.4f), FAUSTFLOAT(-0.8f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->declare(&fHslider6, "name", "p1Gain");
		ui_interface->declare(&fHslider6, "style", "knob");
		ui_interface->addHorizontalSlider("Presence", &fHslider6, FAUSTFLOAT(0.0f), FAUSTFLOAT(-15.0f), FAUSTFLOAT(15.0f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fHslider5, "style", "knob");
		ui_interface->addHorizontalSlider("Saturation dry wet", &fHslider5, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.001f));
		ui_interface->addCheckButton("bypass", &fCheckbox1);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("6 Reverb");
		ui_interface->declare(&fHslider12, "style", "knob");
		ui_interface->addHorizontalSlider("Mix", &fHslider12, FAUSTFLOAT(0.597f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.001f));
		ui_interface->declare(&fEntry5, "style", "menu{'Freeverb':0;'Spring':1;'Zita':2}");
		ui_interface->addNumEntry("Type", &fEntry5, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(2.0f), FAUSTFLOAT(1.0f));
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Cabinet Simulator");
		ui_interface->declare(&fCheckbox0, "stratus", "0");
		ui_interface->addCheckButton("Bypass", &fCheckbox0);
		ui_interface->declare(&fEntry6, "stratus", "1");
		ui_interface->declare(&fEntry6, "style", "menu{'fenderDeluxeJensen1x12':0;'Mesa-OS-Rectifier-3':1;'EV MIX D':2}");
		ui_interface->addNumEntry("Cabinet Select", &fEntry6, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(2.0f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider1, "stratus", "2");
		ui_interface->addHorizontalSlider("Mix (Dry/Wet)", &fHslider1, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->declare(&fHslider0, "stratus", "3");
		ui_interface->addHorizontalSlider("Output (dB)", &fHslider0, FAUSTFLOAT(0.0f), FAUSTFLOAT(-2e+01f), FAUSTFLOAT(1e+01f), FAUSTFLOAT(0.1f));
		ui_interface->closeBox();
		ui_interface->declare(0, "stratus", "4");
		ui_interface->openHorizontalBox("Output");
		ui_interface->declare(&fHslider13, "style", "knob");
		ui_interface->addHorizontalSlider("Level", &fHslider13, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Preamp Guitarix");
		ui_interface->openHorizontalBox("Preamp Guitarix");
		ui_interface->declare(&fHslider4, "style", "knob");
		ui_interface->addHorizontalSlider("Input Volume", &fHslider4, FAUSTFLOAT(-6.0f), FAUSTFLOAT(-2e+01f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fHslider3, "style", "knob");
		ui_interface->addHorizontalSlider("Interstage gain", &fHslider3, FAUSTFLOAT(-6.0f), FAUSTFLOAT(-2e+01f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fEntry1, "style", "menu{'1 stage':0;'2 stages':1;'3 stages':2}");
		ui_interface->addNumEntry("Nb Stages", &fEntry1, FAUSTFLOAT(2.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(2.0f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Output gain", &fHslider2, FAUSTFLOAT(7.0f), FAUSTFLOAT(-2e+01f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fEntry2, "style", "menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}");
		ui_interface->addNumEntry("Stage 1 Tube", &fEntry2, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(5.0f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fEntry3, "style", "menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}");
		ui_interface->addNumEntry("Stage 2 Tube", &fEntry3, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(5.0f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fEntry4, "style", "menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}");
		ui_interface->addNumEntry("Stage 3 Tube", &fEntry4, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(5.0f), FAUSTFLOAT(1.0f));
		ui_interface->closeBox();
		ui_interface->declare(&fHbargraph0, "0", "");
		ui_interface->declare(&fHbargraph0, "unit", "dB");
		ui_interface->addHorizontalBargraph("Preamp Guitarix/Input", &fHbargraph0, FAUSTFLOAT(-6e+01f), FAUSTFLOAT(1e+01f));
		ui_interface->declare(&fHbargraph1, "1", "");
		ui_interface->declare(&fHbargraph1, "unit", "dB");
		ui_interface->addHorizontalBargraph("Preamp Guitarix/Inter 1-2", &fHbargraph1, FAUSTFLOAT(-6e+01f), FAUSTFLOAT(1e+01f));
		ui_interface->declare(&fHbargraph2, "2", "");
		ui_interface->declare(&fHbargraph2, "unit", "dB");
		ui_interface->addHorizontalBargraph("Preamp Guitarix/Output", &fHbargraph2, FAUSTFLOAT(-6e+01f), FAUSTFLOAT(1e+01f));
		ui_interface->addCheckButton("bypass", &fCheckbox3);
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		int iSlow0 = static_cast<int>(static_cast<float>(fCheckbox0));
		float fSlow1 = fConst1 * std::pow(1e+01f, 0.05f * static_cast<float>(fHslider0));
		float fSlow2 = fConst1 * static_cast<float>(fHslider1);
		float fSlow3 = static_cast<float>(fCheckbox1);
		int iSlow4 = static_cast<int>(static_cast<float>(fEntry0));
		int iSlow5 = iSlow4 >= 2;
		int iSlow6 = iSlow4 >= 1;
		int iSlow7 = iSlow4 >= 3;
		float fSlow8 = ((iSlow5) ? ((iSlow7) ? 6.1f : 5.48f) : ((iSlow6) ? 2.13f : 6.93f));
		float fSlow9 = static_cast<float>(fCheckbox2);
		float fSlow10 = static_cast<float>(fVslider0);
		float fSlow11 = 0.001175f * fSlow10;
		float fSlow12 = std::exp(3.4f * (static_cast<float>(fVslider1) + -1.0f));
		float fSlow13 = 0.0250625f * fSlow12;
		float fSlow14 = fConst4 * (fSlow11 + fSlow13 + 0.01726875f);
		float fSlow15 = 2.9448438e-06f * fSlow10;
		float fSlow16 = fConst5 * (fSlow10 * (2.9448438e-05f * fSlow12 + -8.7317185e-06f - fSlow15) + 0.00011998125f * fSlow12 + 1.2916875e-05f);
		float fSlow17 = fSlow14 + fSlow16;
		float fSlow18 = 2.5703124e-09f * fSlow12 - 2.5703126e-10f * fSlow10;
		float fSlow19 = 7.34375e-09f * fSlow12;
		float fSlow20 = fConst6 * (fSlow10 * (fSlow18 + -4.773438e-10f) + fSlow19 + 7.34375e-10f);
		float fSlow21 = 1.0f / (-1.0f - (fSlow17 + fSlow20));
		float fSlow22 = static_cast<float>(fVslider2);
		float fSlow23 = fConst5 * (9.1875e-07f * fSlow22 + fSlow10 * (3.0182812e-06f - fSlow15) + fSlow12 * (2.9448438e-05f * fSlow10 + 2.48125e-06f) + 2.48125e-07f);
		float fSlow24 = 1.0f - fSlow10;
		float fSlow25 = fSlow10 * (fSlow18 + 2.5703126e-10f) + fSlow22 * (fSlow19 + 7.34375e-10f * fSlow24);
		float fSlow26 = fConst7 * fSlow25;
		float fSlow27 = 6.25e-05f * fSlow22;
		float fSlow28 = fConst4 * (fSlow13 + fSlow11 + fSlow27 + 0.00250625f);
		float fSlow29 = fSlow23 + fSlow26 - fSlow28;
		float fSlow30 = 0.001f * std::pow(1e+01f, 0.05f * static_cast<float>(fHslider2));
		float fSlow31 = static_cast<float>(fEntry1);
		int iSlow32 = fSlow31 > 0.0f;
		float fSlow33 = 0.001f * std::pow(1e+01f, 0.05f * static_cast<float>(fHslider3));
		float fSlow34 = static_cast<float>(fCheckbox3);
		int iSlow35 = fSlow31 > 1.0f;
		float fSlow36 = static_cast<float>(fEntry2);
		int iSlow37 = fSlow36 >= 3.0f;
		int iSlow38 = fSlow36 >= 2.0f;
		int iSlow39 = fSlow36 >= 1.0f;
		float fSlow40 = 0.001f * std::pow(1e+01f, 0.05f * static_cast<float>(fHslider4));
		int iSlow41 = fSlow36 >= 5.0f;
		int iSlow42 = fSlow36 >= 4.0f;
		float fSlow43 = static_cast<float>(fEntry3);
		int iSlow44 = fSlow43 >= 3.0f;
		int iSlow45 = fSlow43 >= 2.0f;
		int iSlow46 = fSlow43 >= 1.0f;
		int iSlow47 = fSlow43 >= 5.0f;
		int iSlow48 = fSlow43 >= 4.0f;
		float fSlow49 = static_cast<float>(fEntry4);
		int iSlow50 = fSlow49 >= 3.0f;
		int iSlow51 = fSlow49 >= 2.0f;
		int iSlow52 = fSlow49 >= 1.0f;
		int iSlow53 = fSlow49 >= 5.0f;
		int iSlow54 = fSlow49 >= 4.0f;
		float fSlow55 = fSlow16 - (fSlow14 + 3.0f * (1.0f - fSlow20));
		float fSlow56 = fSlow17 - 3.0f * (fSlow20 + 1.0f);
		float fSlow57 = fSlow14 + fSlow20 + (-1.0f - fSlow16);
		float fSlow58 = fSlow28 + fSlow23;
		float fSlow59 = fSlow58 - fSlow26;
		float fSlow60 = fConst6 * fSlow25;
		float fSlow61 = fSlow28 + fSlow60 - fSlow23;
		float fSlow62 = fSlow58 + fSlow60;
		float fSlow63 = 0.000484f * fSlow10;
		float fSlow64 = 0.02247f * fSlow12;
		float fSlow65 = fConst4 * (fSlow63 + fSlow64 + 0.00204974f);
		float fSlow66 = 2.3926057e-07f * fSlow10;
		float fSlow67 = fConst5 * (fSlow10 * (1.087548e-05f * fSlow12 + -6.207784e-08f - fSlow66) + 1.892924e-05f * fSlow12 + 5.6658007e-07f);
		float fSlow68 = fSlow65 + fSlow67;
		float fSlow69 = 1.2661536e-09f * fSlow12 - 2.7855381e-11f * fSlow10;
		float fSlow70 = 1.6515048e-09f * fSlow12;
		float fSlow71 = fConst6 * (fSlow10 * (fSlow69 + -8.477725e-12f) + fSlow70 + 3.6333107e-11f);
		float fSlow72 = 1.0f / (-1.0f - (fSlow68 + fSlow71));
		float fSlow73 = fConst5 * (1.501368e-07f * fSlow22 + fSlow10 * (2.8930617e-07f - fSlow66) + fSlow12 * (1.087548e-05f * fSlow10 + 2.95724e-06f) + 6.505928e-08f);
		float fSlow74 = fSlow10 * (fSlow69 + 2.7855381e-11f) + fSlow22 * (fSlow70 + 3.6333107e-11f * fSlow24);
		float fSlow75 = fConst7 * fSlow74;
		float fSlow76 = fConst4 * (fSlow64 + fSlow63 + 0.0001034f * fSlow22 + 0.00049434f);
		float fSlow77 = fSlow73 + fSlow75 - fSlow76;
		float fSlow78 = fSlow67 - (fSlow65 + 3.0f * (1.0f - fSlow71));
		float fSlow79 = fSlow68 - 3.0f * (fSlow71 + 1.0f);
		float fSlow80 = fSlow65 + fSlow71 + (-1.0f - fSlow67);
		float fSlow81 = fSlow76 + fSlow73;
		float fSlow82 = fSlow81 - fSlow75;
		float fSlow83 = fConst6 * fSlow74;
		float fSlow84 = fSlow76 + fSlow83 - fSlow73;
		float fSlow85 = fSlow81 + fSlow83;
		float fSlow86 = 0.00022f * fSlow10;
		float fSlow87 = 0.02205f * fSlow12;
		float fSlow88 = fConst4 * (fSlow86 + fSlow87 + 0.0046705f);
		float fSlow89 = 4.851e-08f * fSlow10;
		float fSlow90 = fConst5 * (fSlow10 * (4.851e-06f * fSlow12 + -4.2449e-07f - fSlow89) + 4.972e-05f * fSlow12 + 7.172e-07f);
		float fSlow91 = fSlow88 + fSlow90;
		float fSlow92 = 2.662e-10f * fSlow12 - 2.662e-12f * fSlow10;
		float fSlow93 = 2.42e-09f * fSlow12;
		float fSlow94 = fConst6 * (fSlow10 * (fSlow92 + -2.1538e-11f) + fSlow93 + 2.42e-11f);
		float fSlow95 = 1.0f / (-1.0f - (fSlow91 + fSlow94));
		float fSlow96 = fConst5 * (2.2e-07f * fSlow22 + fSlow10 * (5.951e-08f - fSlow89) + fSlow12 * (4.851e-06f * fSlow10 + 1.32e-06f) + 1.32e-08f);
		float fSlow97 = fSlow10 * (fSlow92 + 2.662e-12f) + fSlow22 * (fSlow93 + 2.42e-11f * fSlow24);
		float fSlow98 = fConst7 * fSlow97;
		float fSlow99 = fConst4 * (fSlow87 + fSlow86 + 5e-05f * fSlow22 + 0.0002205f);
		float fSlow100 = fSlow96 + fSlow98 - fSlow99;
		float fSlow101 = fSlow90 - (fSlow88 + 3.0f * (1.0f - fSlow94));
		float fSlow102 = fSlow91 - 3.0f * (fSlow94 + 1.0f);
		float fSlow103 = fSlow88 + fSlow94 + (-1.0f - fSlow90);
		float fSlow104 = fSlow99 + fSlow96;
		float fSlow105 = fSlow104 - fSlow98;
		float fSlow106 = fConst6 * fSlow97;
		float fSlow107 = fSlow99 + fSlow106 - fSlow96;
		float fSlow108 = fSlow104 + fSlow106;
		float fSlow109 = 0.00055f * fSlow10;
		float fSlow110 = fConst4 * (fSlow13 + fSlow109 + 0.01842875f);
		float fSlow111 = 1.3784374e-06f * fSlow10;
		float fSlow112 = fConst5 * (fSlow10 * (1.3784375e-05f * fSlow12 + -5.7371876e-06f - fSlow111) + 7.405375e-05f * fSlow12 + 8.396625e-06f);
		float fSlow113 = fSlow110 + fSlow112;
		float fSlow114 = 1.30625e-09f * fSlow12 - 1.30625e-10f * fSlow10;
		float fSlow115 = 4.46875e-09f * fSlow12;
		float fSlow116 = fConst6 * (fSlow10 * (fSlow114 + -3.1625e-10f) + fSlow115 + 4.46875e-10f);
		float fSlow117 = 1.0f / (-1.0f - (fSlow113 + fSlow116));
		float fSlow118 = fConst5 * (9.9125e-07f * fSlow22 + fSlow10 * (1.4128125e-06f - fSlow111) + fSlow12 * (1.3784375e-05f * fSlow10 + 2.55375e-06f) + 2.55375e-07f);
		float fSlow119 = fSlow10 * (fSlow114 + 1.30625e-10f) + fSlow22 * (fSlow115 + 4.46875e-10f * fSlow24);
		float fSlow120 = fConst7 * fSlow119;
		float fSlow121 = fConst4 * (fSlow13 + fSlow27 + fSlow109 + 0.00250625f);
		float fSlow122 = fSlow118 + fSlow120 - fSlow121;
		float fSlow123 = fSlow112 - (fSlow110 + 3.0f * (1.0f - fSlow116));
		float fSlow124 = fSlow113 - 3.0f * (fSlow116 + 1.0f);
		float fSlow125 = fSlow110 + fSlow116 + (-1.0f - fSlow112);
		float fSlow126 = fSlow121 + fSlow118;
		float fSlow127 = fSlow126 - fSlow120;
		float fSlow128 = fConst6 * fSlow119;
		float fSlow129 = fSlow121 + fSlow128 - fSlow118;
		float fSlow130 = fSlow126 + fSlow128;
		float fSlow131 = 0.005f * static_cast<float>(fHslider5);
		float fSlow132 = std::pow(1e+01f, 0.025f * static_cast<float>(fHslider6));
		float fSlow133 = fConst94 / fSlow132;
		float fSlow134 = 1.0f / (fSlow133 + 1.0f);
		float fSlow135 = fConst1 * static_cast<float>(fHslider7);
		float fSlow136 = fConst94 * fSlow132;
		float fSlow137 = fSlow136 + 1.0f;
		float fSlow138 = fConst96 / fSlow132;
		float fSlow139 = 1.0f / (fSlow138 + 1.0f);
		float fSlow140 = fConst96 * fSlow132;
		float fSlow141 = fSlow140 + 1.0f;
		float fSlow142 = 1.0f - fSlow138;
		float fSlow143 = 1.0f - fSlow140;
		float fSlow144 = 1.0f - fSlow133;
		float fSlow145 = 1.0f - fSlow136;
		float fSlow146 = fConst1 * static_cast<float>(fHslider8);
		float fSlow147 = 0.005f * static_cast<float>(fHslider9);
		float fSlow148 = 0.005f * static_cast<float>(fHslider10);
		float fSlow149 = 0.005f * std::pow(1e+01f, 0.05f * static_cast<float>(fHslider11));
		float fSlow150 = fConst1 * static_cast<float>(fHslider12);
		float fSlow151 = fConst1 * static_cast<float>(fHslider13);
		int iSlow152 = static_cast<int>(static_cast<float>(fEntry5));
		int iSlow153 = iSlow152 == 0;
		int iSlow154 = iSlow152 == 1;
		float fSlow155 = static_cast<float>(fEntry6);
		float fSlow156 = ((fSlow155 >= 2.0f) ? 0.21845f : ((fSlow155 >= 1.0f) ? 0.171461f : 0.155734f));
		float fSlow157 = static_cast<float>(fSlow155 == 1.0f);
		float fSlow158 = static_cast<float>(fSlow155 == 0.0f);
		float fSlow159 = static_cast<float>(fSlow155 == 2.0f);
		float fSlow160 = 0.01059052f * fSlow157 - 0.00132043f * fSlow158 + 0.07774543f * fSlow159;
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			fRec0[0] = fSlow1 + fConst2 * fRec0[1];
			fRec1[0] = fSlow2 + fConst2 * fRec1[1];
			float fTemp0 = fConst3 + fRec3[1];
			float fTemp1 = fRec3[1] - fConst3;
			fRec3[0] = ((fTemp0 < fSlow3) ? fTemp0 : ((fTemp1 > fSlow3) ? fTemp1 : fSlow3));
			float fTemp2 = fConst3 + fRec4[1];
			float fTemp3 = fRec4[1] - fConst3;
			fRec4[0] = ((fTemp2 < fSlow9) ? fTemp2 : ((fTemp3 > fSlow9) ? fTemp3 : fSlow9));
			float fTemp4 = 1.0f - fRec4[0];
			fRec11[0] = fSlow30 + 0.999f * fRec11[1];
			fRec13[0] = fSlow33 + 0.999f * fRec13[1];
			fRec12[0] = 0.001f * ((iSlow32) ? 0.5011872f / std::max<float>(0.001f, fRec13[0]) : 1.0f) + 0.999f * fRec12[1];
			float fTemp5 = fConst3 + fRec14[1];
			float fTemp6 = fRec14[1] - fConst3;
			fRec14[0] = ((fTemp5 < fSlow34) ? fTemp5 : ((fTemp6 > fSlow34) ? fTemp6 : fSlow34));
			float fTemp7 = 1.0f - fRec14[0];
			fRec18[0] = fConst32 * (0.027f * (fRec17[1] + fRec17[2]) - fConst33 * fRec18[1]);
			fRec20[0] = fSlow40 + 0.999f * fRec20[1];
			float fTemp8 = static_cast<float>(input0[i0]);
			fVec0[0] = fTemp8;
			fRec23[0] = fTemp8 + 0.995f * fRec23[1] - fVec0[1];
			fRec22[0] = -(fConst41 * (fConst42 * fRec22[1] - fConst40 * (fRec23[0] - fRec23[1])));
			float fTemp9 = fConst39 * fRec21[1];
			fRec21[0] = fRec22[0] - fConst38 * (fConst43 * fRec21[2] + fTemp9);
			float fTemp10 = fTemp9 + fConst45 * fRec21[0] + fConst46 * fRec21[2];
			float fTemp11 = fConst38 * fRec20[0] * fTemp7 * fTemp10;
			fRec19[0] = std::max<float>(fConst26 * fRec19[1], std::fabs(fTemp11));
			fHbargraph0 = static_cast<FAUSTFLOAT>(2e+01f * std::log10(std::max<float>(1.1754944e-38f, fRec19[0])));
			float fTemp12 = fTemp11;
			float fTemp13 = 2e+02f * (fRec18[0] + fTemp12 + 3.418344f);
			int iTemp14 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp13)));
			float fTemp15 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp13 - static_cast<float>(iTemp14)));
			fRec17[0] = ((fTemp13 < 0.0f) ? fConst48 : ((fTemp13 >= 2e+03f) ? fConst47 : ftbl0mydspSIG0[iTemp14] * (1.0f - fTemp15) + fTemp15 * ftbl0mydspSIG0[iTemp14 + 1])) + -191.42015f;
			fRec16[0] = fConst29 * (fConst30 * (fRec17[0] - fRec17[1]) - fConst49 * fRec16[1]);
			fRec26[0] = fConst32 * (0.027f * (fRec25[1] + fRec25[2]) - fConst33 * fRec26[1]);
			float fTemp16 = 2e+02f * (fRec26[0] + fTemp12 + 2.382247f);
			int iTemp17 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp16)));
			float fTemp18 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp16 - static_cast<float>(iTemp17)));
			fRec25[0] = ((fTemp16 < 0.0f) ? fConst51 : ((fTemp16 >= 2e+03f) ? fConst50 : ftbl1mydspSIG1[iTemp17] * (1.0f - fTemp18) + fTemp18 * ftbl1mydspSIG1[iTemp17 + 1])) + -153.04619f;
			fRec24[0] = fConst29 * (fConst30 * (fRec25[0] - fRec25[1]) - fConst49 * fRec24[1]);
			fRec29[0] = fConst32 * (0.027f * (fRec28[1] + fRec28[2]) - fConst33 * fRec29[1]);
			float fTemp19 = 2e+02f * (fRec29[0] + fTemp12 + 1.281038f);
			int iTemp20 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp19)));
			float fTemp21 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp19 - static_cast<float>(iTemp20)));
			fRec28[0] = ((fTemp19 < 0.0f) ? fConst53 : ((fTemp19 >= 2e+03f) ? fConst52 : ftbl2mydspSIG2[iTemp20] * (1.0f - fTemp21) + fTemp21 * ftbl2mydspSIG2[iTemp20 + 1])) + -112.260666f;
			fRec27[0] = fConst29 * (fConst30 * (fRec28[0] - fRec28[1]) - fConst49 * fRec27[1]);
			fRec32[0] = fConst32 * (0.027f * (fRec31[1] + fRec31[2]) - fConst33 * fRec32[1]);
			float fTemp22 = 2e+02f * (fRec32[0] + fTemp12 + 2.70385f);
			int iTemp23 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp22)));
			float fTemp24 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp22 - static_cast<float>(iTemp23)));
			fRec31[0] = ((fTemp22 < 0.0f) ? fConst55 : ((fTemp22 >= 2e+03f) ? fConst54 : ftbl3mydspSIG3[iTemp23] * (1.0f - fTemp24) + fTemp24 * ftbl3mydspSIG3[iTemp23 + 1])) + -164.95741f;
			fRec30[0] = fConst29 * (fConst30 * (fRec31[0] - fRec31[1]) - fConst49 * fRec30[1]);
			fRec35[0] = fConst32 * (0.027f * (fRec34[1] + fRec34[2]) - fConst33 * fRec35[1]);
			float fTemp25 = 2e+02f * (fRec35[0] + fTemp12 + 3.136054f);
			int iTemp26 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp25)));
			float fTemp27 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp25 - static_cast<float>(iTemp26)));
			fRec34[0] = ((fTemp25 < 0.0f) ? fConst58 : ((fTemp25 >= 2e+03f) ? fConst57 : ftbl4mydspSIG4[iTemp26] * (1.0f - fTemp27) + fTemp27 * ftbl4mydspSIG4[iTemp26 + 1])) + -60.964962f;
			fRec33[0] = fConst29 * (fConst56 * (fRec34[0] - fRec34[1]) - fConst49 * fRec33[1]);
			fRec38[0] = fConst32 * (0.027f * (fRec37[1] + fRec37[2]) - fConst33 * fRec38[1]);
			float fTemp28 = 2e+02f * (fRec38[0] + fTemp12 + 2.078194f);
			int iTemp29 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp28)));
			float fTemp30 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp28 - static_cast<float>(iTemp29)));
			fRec37[0] = ((fTemp28 < 0.0f) ? fConst60 : ((fTemp28 >= 2e+03f) ? fConst59 : ftbl5mydspSIG5[iTemp29] * (1.0f - fTemp30) + fTemp30 * ftbl5mydspSIG5[iTemp29 + 1])) + -141.78496f;
			fRec36[0] = fConst29 * (fConst30 * (fRec37[0] - fRec37[1]) - fConst49 * fRec36[1]);
			float fTemp31 = ((iSlow37) ? ((iSlow41) ? fRec36[0] : ((iSlow42) ? fRec33[0] : fRec30[0])) : ((iSlow38) ? fRec27[0] : ((iSlow39) ? fRec24[0] : fRec16[0])));
			fRec15[0] = std::max<float>(fConst26 * fRec15[1], std::fabs(fTemp31));
			fHbargraph1 = static_cast<FAUSTFLOAT>(2e+01f * std::log10(std::max<float>(1.1754944e-38f, fRec15[0])));
			float fTemp32 = fTemp31;
			fRec42[0] = fConst62 * (0.015f * (fRec41[1] + fRec41[2]) - fConst63 * fRec42[1]);
			float fTemp33 = fRec13[0] * fTemp32;
			fVec1[0] = fTemp33;
			fRec43[0] = -(fConst65 * (fConst66 * fRec43[1] - (fTemp33 + fVec1[1])));
			float fTemp34 = 2e+02f * (fRec42[0] + fRec43[0] + 3.795715f);
			int iTemp35 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp34)));
			float fTemp36 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp34 - static_cast<float>(iTemp35)));
			fRec41[0] = ((fTemp34 < 0.0f) ? fConst68 : ((fTemp34 >= 2e+03f) ? fConst67 : ftbl6mydspSIG6[iTemp35] * (1.0f - fTemp36) + fTemp36 * ftbl6mydspSIG6[iTemp35 + 1])) + -169.71434f;
			fRec40[0] = fConst29 * (fConst30 * (fRec41[0] - fRec41[1]) - fConst49 * fRec40[1]);
			fRec46[0] = fConst62 * (0.015f * (fRec45[1] + fRec45[2]) - fConst63 * fRec46[1]);
			float fTemp37 = 2e+02f * (fRec43[0] + fRec46[0] + 3.112668f);
			int iTemp38 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp37)));
			float fTemp39 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp37 - static_cast<float>(iTemp38)));
			fRec45[0] = ((fTemp37 < 0.0f) ? fConst70 : ((fTemp37 >= 2e+03f) ? fConst69 : ftbl7mydspSIG7[iTemp38] * (1.0f - fTemp39) + fTemp39 * ftbl7mydspSIG7[iTemp38 + 1])) + -124.177864f;
			fRec44[0] = fConst29 * (fConst30 * (fRec45[0] - fRec45[1]) - fConst49 * fRec44[1]);
			fRec49[0] = fConst62 * (0.015f * (fRec48[1] + fRec48[2]) - fConst63 * fRec49[1]);
			float fTemp40 = 2e+02f * (fRec43[0] + fRec49[0] + 2.685156f);
			int iTemp41 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp40)));
			float fTemp42 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp40 - static_cast<float>(iTemp41)));
			fRec48[0] = ((fTemp40 < 0.0f) ? fConst72 : ((fTemp40 >= 2e+03f) ? fConst71 : ftbl8mydspSIG8[iTemp41] * (1.0f - fTemp42) + fTemp42 * ftbl8mydspSIG8[iTemp41 + 1])) + -95.67706f;
			fRec47[0] = fConst29 * (fConst30 * (fRec48[0] - fRec48[1]) - fConst49 * fRec47[1]);
			fRec52[0] = fConst62 * (0.015f * (fRec51[1] + fRec51[2]) - fConst63 * fRec52[1]);
			float fTemp43 = 2e+02f * (fRec43[0] + fRec52[0] + 3.324413f);
			int iTemp44 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp43)));
			float fTemp45 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp43 - static_cast<float>(iTemp44)));
			fRec51[0] = ((fTemp43 < 0.0f) ? fConst74 : ((fTemp43 >= 2e+03f) ? fConst73 : ftbl9mydspSIG9[iTemp44] * (1.0f - fTemp45) + fTemp45 * ftbl9mydspSIG9[iTemp44 + 1])) + -138.2942f;
			fRec50[0] = fConst29 * (fConst30 * (fRec51[0] - fRec51[1]) - fConst49 * fRec50[1]);
			fRec55[0] = fConst62 * (0.015f * (fRec54[1] + fRec54[2]) - fConst63 * fRec55[1]);
			float fTemp46 = 2e+02f * (fRec43[0] + fRec55[0] + 3.728391f);
			int iTemp47 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp46)));
			float fTemp48 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp46 - static_cast<float>(iTemp47)));
			fRec54[0] = ((fTemp46 < 0.0f) ? fConst76 : ((fTemp46 >= 2e+03f) ? fConst75 : ftbl10mydspSIG10[iTemp47] * (1.0f - fTemp48) + fTemp48 * ftbl10mydspSIG10[iTemp47 + 1])) + -45.226067f;
			fRec53[0] = fConst29 * (fConst56 * (fRec54[0] - fRec54[1]) - fConst49 * fRec53[1]);
			fRec58[0] = fConst62 * (0.015f * (fRec57[1] + fRec57[2]) - fConst63 * fRec58[1]);
			float fTemp49 = 2e+02f * (fRec43[0] + fRec58[0] + 2.902257f);
			int iTemp50 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp49)));
			float fTemp51 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp49 - static_cast<float>(iTemp50)));
			fRec57[0] = ((fTemp49 < 0.0f) ? fConst78 : ((fTemp49 >= 2e+03f) ? fConst77 : ftbl11mydspSIG11[iTemp50] * (1.0f - fTemp51) + fTemp51 * ftbl11mydspSIG11[iTemp50 + 1])) + -110.15047f;
			fRec56[0] = fConst29 * (fConst30 * (fRec57[0] - fRec57[1]) - fConst49 * fRec56[1]);
			float fTemp52 = ((iSlow44) ? ((iSlow47) ? fRec56[0] : ((iSlow48) ? fRec53[0] : fRec50[0])) : ((iSlow45) ? fRec47[0] : ((iSlow46) ? fRec44[0] : fRec40[0])));
			fRec39[0] = std::max<float>(fConst26 * fRec39[1], std::fabs(fTemp52));
			fHbargraph2 = static_cast<FAUSTFLOAT>(2e+01f * std::log10(std::max<float>(1.1754944e-38f, fRec39[0])));
			float fTemp53 = ((iSlow32) ? fTemp52 : fTemp32);
			fRec61[0] = fConst80 * (0.0082f * (fRec60[1] + fRec60[2]) - fConst81 * fRec61[1]);
			float fTemp54 = fRec13[0] * fTemp53;
			fVec2[0] = fTemp54;
			fRec62[0] = -(fConst65 * (fConst66 * fRec62[1] - (fTemp54 + fVec2[1])));
			float fTemp55 = 2e+02f * (fRec61[0] + fRec62[0] + 4.159297f);
			int iTemp56 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp55)));
			float fTemp57 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp55 - static_cast<float>(iTemp56)));
			fRec60[0] = ((fTemp55 < 0.0f) ? fConst68 : ((fTemp55 >= 2e+03f) ? fConst67 : ftbl6mydspSIG6[iTemp56] * (1.0f - fTemp57) + fTemp57 * ftbl6mydspSIG6[iTemp56 + 1])) + -147.47525f;
			fRec59[0] = fConst29 * (fConst30 * (fRec60[0] - fRec60[1]) - fConst49 * fRec59[1]);
			fRec65[0] = fConst80 * (0.0082f * (fRec64[1] + fRec64[2]) - fConst81 * fRec65[1]);
			float fTemp58 = 2e+02f * (fRec62[0] + fRec65[0] + 3.743038f);
			int iTemp59 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp58)));
			float fTemp60 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp58 - static_cast<float>(iTemp59)));
			fRec64[0] = ((fTemp58 < 0.0f) ? fConst70 : ((fTemp58 >= 2e+03f) ? fConst69 : ftbl7mydspSIG7[iTemp59] * (1.0f - fTemp60) + fTemp60 * ftbl7mydspSIG7[iTemp59 + 1])) + -96.71195f;
			fRec63[0] = fConst29 * (fConst30 * (fRec64[0] - fRec64[1]) - fConst49 * fRec63[1]);
			fRec68[0] = fConst80 * (0.0082f * (fRec67[1] + fRec67[2]) - fConst81 * fRec68[1]);
			float fTemp61 = 2e+02f * (fRec62[0] + fRec68[0] + 3.643433f);
			int iTemp62 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp61)));
			float fTemp63 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp61 - static_cast<float>(iTemp62)));
			fRec67[0] = ((fTemp61 < 0.0f) ? fConst72 : ((fTemp61 >= 2e+03f) ? fConst71 : ftbl8mydspSIG8[iTemp62] * (1.0f - fTemp63) + fTemp63 * ftbl8mydspSIG8[iTemp62 + 1])) + -84.565f;
			fRec66[0] = fConst29 * (fConst30 * (fRec67[0] - fRec67[1]) - fConst49 * fRec66[1]);
			fRec71[0] = fConst80 * (0.0082f * (fRec70[1] + fRec70[2]) - fConst81 * fRec71[1]);
			float fTemp64 = 2e+02f * (fRec62[0] + fRec71[0] + 3.869538f);
			int iTemp65 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp64)));
			float fTemp66 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp64 - static_cast<float>(iTemp65)));
			fRec70[0] = ((fTemp64 < 0.0f) ? fConst74 : ((fTemp64 >= 2e+03f) ? fConst73 : ftbl9mydspSIG9[iTemp65] * (1.0f - fTemp66) + fTemp66 * ftbl9mydspSIG9[iTemp65 + 1])) + -112.13878f;
			fRec69[0] = fConst29 * (fConst30 * (fRec70[0] - fRec70[1]) - fConst49 * fRec69[1]);
			fRec74[0] = fConst80 * (0.0082f * (fRec73[1] + fRec73[2]) - fConst81 * fRec74[1]);
			float fTemp67 = 2e+02f * (fRec62[0] + fRec74[0] + 4.200969f);
			int iTemp68 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp67)));
			float fTemp69 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp67 - static_cast<float>(iTemp68)));
			fRec73[0] = ((fTemp67 < 0.0f) ? fConst58 : ((fTemp67 >= 2e+03f) ? fConst57 : ftbl4mydspSIG4[iTemp68] * (1.0f - fTemp69) + fTemp69 * ftbl4mydspSIG4[iTemp68 + 1])) + -32.557194f;
			fRec72[0] = fConst29 * (fConst56 * (fRec73[0] - fRec73[1]) - fConst49 * fRec72[1]);
			fRec77[0] = fConst80 * (0.0082f * (fRec76[1] + fRec76[2]) - fConst81 * fRec77[1]);
			float fTemp70 = 2e+02f * (fRec62[0] + fRec77[0] + 3.621258f);
			int iTemp71 = std::max<int>(0, std::min<int>(1999, static_cast<int>(fTemp70)));
			float fTemp72 = std::max<float>(0.0f, std::min<float>(1.0f, fTemp70 - static_cast<float>(iTemp71)));
			fRec76[0] = ((fTemp70 < 0.0f) ? fConst78 : ((fTemp70 >= 2e+03f) ? fConst77 : ftbl11mydspSIG11[iTemp71] * (1.0f - fTemp72) + fTemp72 * ftbl11mydspSIG11[iTemp71 + 1])) + -81.86073f;
			fRec75[0] = fConst29 * (fConst30 * (fRec76[0] - fRec76[1]) - fConst49 * fRec75[1]);
			float fTemp73 = fRec11[0] * fRec12[0] * fTemp7 * ((iSlow35) ? ((iSlow50) ? ((iSlow53) ? fRec75[0] : ((iSlow54) ? fRec72[0] : fRec69[0])) : ((iSlow51) ? fRec66[0] : ((iSlow52) ? fRec63[0] : fRec59[0]))) : fTemp53) + fConst38 * fRec14[0] * fTemp10;
			fVec3[0] = fTemp73;
			fRec10[0] = -(fConst24 * (fConst25 * fRec10[1] - fConst21 * (fTemp73 - fVec3[1])));
			fRec9[0] = fRec10[0] - fConst22 * (fConst82 * fRec9[2] + fConst83 * fRec9[1]);
			fRec79[0] = -(fConst24 * (fConst25 * fRec79[1] - (fTemp73 + fVec3[1])));
			fRec78[0] = fRec79[0] - fConst22 * (fConst82 * fRec78[2] + fConst83 * fRec78[1]);
			float fTemp74 = fConst19 * fRec8[1];
			fRec8[0] = fConst22 * (fConst23 * (fRec9[2] + (fRec9[0] - 2.0f * fRec9[1])) + 2.6668587f * (fRec78[2] + fRec78[0] + 2.0f * fRec78[1])) - fConst18 * (fConst84 * fRec8[2] + fTemp74);
			float fTemp75 = fTemp74 + fConst86 * fRec8[0] + fConst87 * fRec8[2];
			fVec4[0] = fTemp75;
			fRec7[0] = -(fConst11 * (fConst12 * fRec7[1] - fConst18 * (fTemp75 + fVec4[1])));
			fRec6[0] = fRec7[0] - fConst10 * (fConst88 * fRec6[2] + fConst90 * fRec6[1]);
			fRec81[0] = -(fConst11 * (fConst12 * fRec81[1] - fConst92 * (fTemp75 - fVec4[1])));
			fRec80[0] = fRec81[0] - fConst10 * (fConst88 * fRec80[2] + fConst90 * fRec80[1]);
			float fTemp76 = fRec6[2] + fRec6[0] + 2.0f * fRec6[1] + fConst91 * (fRec80[2] + (fRec80[0] - 2.0f * fRec80[1]));
			float fTemp77 = fConst10 * fTemp4 * fTemp76;
			fRec5[0] = fTemp77 - fSlow21 * (fSlow55 * fRec5[1] + fSlow56 * fRec5[2] + fSlow57 * fRec5[3]);
			fRec82[0] = fTemp77 - fSlow72 * (fSlow78 * fRec82[1] + fSlow79 * fRec82[2] + fSlow80 * fRec82[3]);
			fRec83[0] = fTemp77 - fSlow95 * (fSlow101 * fRec83[1] + fSlow102 * fRec83[2] + fSlow103 * fRec83[3]);
			fRec84[0] = fTemp77 - fSlow117 * (fSlow123 * fRec84[1] + fSlow124 * fRec84[2] + fSlow125 * fRec84[3]);
			float fTemp78 = fSlow8 * fTemp4 * ((iSlow5) ? ((iSlow7) ? fSlow117 * (fSlow122 * fRec84[1] + fSlow127 * fRec84[2] + fSlow129 * fRec84[3] - fSlow130 * fRec84[0]) : fSlow95 * (fSlow100 * fRec83[1] + fSlow105 * fRec83[2] + fSlow107 * fRec83[3] - fSlow108 * fRec83[0])) : ((iSlow6) ? fSlow72 * (fSlow77 * fRec82[1] + fSlow82 * fRec82[2] + fSlow84 * fRec82[3] - fSlow85 * fRec82[0]) : fSlow21 * (fSlow29 * fRec5[1] + fSlow59 * fRec5[2] + fSlow61 * fRec5[3] - fSlow62 * fRec5[0]))) + fConst10 * fRec4[0] * fTemp76;
			float fTemp79 = fRec3[0] * fTemp78;
			fRec88[0] = fSlow131 + 0.995f * fRec88[1];
			fRec89[0] = fSlow135 + fConst2 * fRec89[1];
			float fTemp80 = fConst97 * fRec91[1];
			fRec91[0] = fRec85[1] - fSlow139 * (fSlow142 * fRec91[2] - fTemp80);
			float fTemp81 = fConst98 * fRec90[1];
			fRec90[0] = fSlow139 * (fSlow141 * fRec91[0] - fTemp80 + fSlow143 * fRec91[2]) - fSlow134 * (fSlow144 * fRec90[2] - fTemp81);
			fRec92[0] = fSlow146 + fConst2 * fRec92[1];
			float fTemp82 = 1.0f - fRec3[0];
			float fTemp83 = fSlow134 * fRec89[0] * (fSlow137 * fRec90[0] - fTemp81 + fSlow145 * fRec90[2]) + fRec92[0] * fTemp82 * fTemp78;
			float fTemp84 = std::fabs(fTemp83);
			fRec93[0] = std::max<float>(fTemp84, fConst99 * fRec93[1] + fConst100 * fTemp84);
			fRec94[0] = fSlow147 + 0.995f * fRec94[1];
			float fTemp85 = std::min<float>(3.0f, std::max<float>(-3.0f, fRec93[0] + fRec94[0] * fTemp83));
			fRec95[0] = fSlow148 + 0.995f * fRec95[1];
			float fTemp86 = mydsp_faustpower2_f(fRec95[0]);
			float fTemp87 = fTemp86 * mydsp_faustpower2_f(fTemp85);
			float fTemp88 = fTemp87 + 27.0f;
			float fTemp89 = 9.0f * fTemp86 + 27.0f;
			float fTemp90 = (9.0f * fTemp87 + 27.0f) * (fTemp86 + 27.0f);
			float fTemp91 = (1.0f - fRec88[0]) * fTemp83 + 0.24f * (fRec88[0] * fTemp85 * fTemp88 * fTemp89 / fTemp90);
			fVec5[0] = fTemp91;
			fRec87[0] = fVec5[1] - 0.24f * (fTemp85 * fTemp88 * fTemp89 * (fRec87[1] - fTemp91) / fTemp90);
			fRec86[0] = fRec87[0] + 0.995f * fRec86[1] - fRec87[1];
			fRec85[0] = fRec86[0];
			fRec96[0] = fSlow149 + 0.995f * fRec96[1];
			float fTemp92 = 4.0f * fRec85[0] * fRec96[0] * fTemp82;
			fVec6[0] = fTemp92 + fTemp79;
			fRec2[0] = fTemp79 + 0.995f * fRec2[1] + fTemp92 - fVec6[1];
			fRec97[0] = fSlow150 + fConst2 * fRec97[1];
			fRec98[0] = fSlow151 + fConst2 * fRec98[1];
			float fTemp93 = std::pow(1e+01f, 0.05f * (24.0f * (fRec98[0] + -0.5f) + -3e+01f));
			fRec110[0] = -(fConst113 * (fConst114 * fRec110[1] - (fRec106[1] + fRec106[2])));
			fRec109[0] = fConst110 * (fRec106[1] + fConst111 * fRec110[0]) + fConst115 * fRec109[1];
			fVec7[IOTA0 & 32767] = 0.35355338f * fRec109[0] + 1e-20f;
			float fTemp94 = 0.6f * fRec107[1] + fVec7[(IOTA0 - iConst117) & 32767];
			float fTemp95 = fRec2[0] * fTemp93;
			fVec8[IOTA0 & 8191] = fTemp95;
			float fTemp96 = 0.3f * fVec8[(IOTA0 - iConst118) & 8191];
			fVec9[IOTA0 & 2047] = fTemp94 - fTemp96;
			fRec107[0] = fVec9[(IOTA0 - iConst119) & 2047];
			float fRec108 = 0.6f * (fTemp96 - fTemp94);
			fRec114[0] = -(fConst113 * (fConst114 * fRec114[1] - (fRec102[1] + fRec102[2])));
			fRec113[0] = fConst128 * (fRec102[1] + fConst129 * fRec114[0]) + fConst130 * fRec113[1];
			fVec10[IOTA0 & 32767] = 0.35355338f * fRec113[0] + 1e-20f;
			float fTemp97 = 0.6f * fRec111[1] + fVec10[(IOTA0 - iConst132) & 32767];
			fVec11[IOTA0 & 4095] = fTemp97 - fTemp96;
			fRec111[0] = fVec11[(IOTA0 - iConst133) & 4095];
			float fRec112 = 0.6f * (fTemp96 - fTemp97);
			fRec118[0] = -(fConst113 * (fConst114 * fRec118[1] - (fRec104[1] + fRec104[2])));
			fRec117[0] = fConst142 * (fRec104[1] + fConst143 * fRec118[0]) + fConst144 * fRec117[1];
			fVec12[IOTA0 & 16383] = 0.35355338f * fRec117[0] + 1e-20f;
			float fTemp98 = fVec12[(IOTA0 - iConst146) & 16383] + fTemp96 + 0.6f * fRec115[1];
			fVec13[IOTA0 & 4095] = fTemp98;
			fRec115[0] = fVec13[(IOTA0 - iConst147) & 4095];
			float fRec116 = -(0.6f * fTemp98);
			fRec122[0] = -(fConst113 * (fConst114 * fRec122[1] - (fRec100[1] + fRec100[2])));
			fRec121[0] = fConst156 * (fRec100[1] + fConst157 * fRec122[0]) + fConst158 * fRec121[1];
			fVec14[IOTA0 & 32767] = 0.35355338f * fRec121[0] + 1e-20f;
			float fTemp99 = fVec14[(IOTA0 - iConst160) & 32767] + fTemp96 + 0.6f * fRec119[1];
			fVec15[IOTA0 & 4095] = fTemp99;
			fRec119[0] = fVec15[(IOTA0 - iConst161) & 4095];
			float fRec120 = -(0.6f * fTemp99);
			fRec126[0] = -(fConst113 * (fConst114 * fRec126[1] - (fRec105[1] + fRec105[2])));
			fRec125[0] = fConst170 * (fRec105[1] + fConst171 * fRec126[0]) + fConst172 * fRec125[1];
			fVec16[IOTA0 & 16383] = 0.35355338f * fRec125[0] + 1e-20f;
			float fTemp100 = fVec16[(IOTA0 - iConst174) & 16383] - (fTemp96 + 0.6f * fRec123[1]);
			fVec17[IOTA0 & 2047] = fTemp100;
			fRec123[0] = fVec17[(IOTA0 - iConst175) & 2047];
			float fRec124 = 0.6f * fTemp100;
			fRec130[0] = -(fConst113 * (fConst114 * fRec130[1] - (fRec101[1] + fRec101[2])));
			fRec129[0] = fConst184 * (fRec101[1] + fConst185 * fRec130[0]) + fConst186 * fRec129[1];
			fVec18[IOTA0 & 16383] = 0.35355338f * fRec129[0] + 1e-20f;
			float fTemp101 = fVec18[(IOTA0 - iConst188) & 16383] - (fTemp96 + 0.6f * fRec127[1]);
			fVec19[IOTA0 & 4095] = fTemp101;
			fRec127[0] = fVec19[(IOTA0 - iConst189) & 4095];
			float fRec128 = 0.6f * fTemp101;
			fRec134[0] = -(fConst113 * (fConst114 * fRec134[1] - (fRec103[1] + fRec103[2])));
			fRec133[0] = fConst198 * (fRec103[1] + fConst199 * fRec134[0]) + fConst200 * fRec133[1];
			fVec20[IOTA0 & 16383] = 0.35355338f * fRec133[0] + 1e-20f;
			float fTemp102 = fTemp96 + fVec20[(IOTA0 - iConst202) & 16383] - 0.6f * fRec131[1];
			fVec21[IOTA0 & 4095] = fTemp102;
			fRec131[0] = fVec21[(IOTA0 - iConst203) & 4095];
			float fRec132 = 0.6f * fTemp102;
			fRec138[0] = -(fConst113 * (fConst114 * fRec138[1] - (fRec99[1] + fRec99[2])));
			fRec137[0] = fConst212 * (fRec99[1] + fConst213 * fRec138[0]) + fConst214 * fRec137[1];
			fVec22[IOTA0 & 16383] = 0.35355338f * fRec137[0] + 1e-20f;
			float fTemp103 = fVec22[(IOTA0 - iConst216) & 16383] + fTemp96 - 0.6f * fRec135[1];
			fVec23[IOTA0 & 2047] = fTemp103;
			fRec135[0] = fVec23[(IOTA0 - iConst217) & 2047];
			float fRec136 = 0.6f * fTemp103;
			float fTemp104 = fRec136 + fRec132;
			float fTemp105 = fRec124 + fRec128 + fTemp104;
			fRec99[0] = fRec107[1] + fRec111[1] + fRec115[1] + fRec119[1] + fRec123[1] + fRec127[1] + fRec131[1] + fRec135[1] + fRec108 + fRec112 + fRec116 + fRec120 + fTemp105;
			fRec100[0] = fRec123[1] + fRec127[1] + fRec131[1] + fRec135[1] + fTemp105 - (fRec107[1] + fRec111[1] + fRec115[1] + fRec119[1] + fRec108 + fRec112 + fRec120 + fRec116);
			float fTemp106 = fRec128 + fRec124;
			fRec101[0] = fRec115[1] + fRec119[1] + fRec131[1] + fRec135[1] + fRec116 + fRec120 + fTemp104 - (fRec107[1] + fRec111[1] + fRec123[1] + fRec127[1] + fRec108 + fRec112 + fTemp106);
			fRec102[0] = fRec107[1] + fRec111[1] + fRec131[1] + fRec135[1] + fRec108 + fRec112 + fTemp104 - (fRec115[1] + fRec119[1] + fRec123[1] + fRec127[1] + fRec116 + fRec120 + fTemp106);
			float fTemp107 = fRec136 + fRec128;
			float fTemp108 = fRec132 + fRec124;
			fRec103[0] = fRec111[1] + fRec119[1] + fRec127[1] + fRec135[1] + fRec112 + fRec120 + fTemp107 - (fRec107[1] + fRec115[1] + fRec123[1] + fRec131[1] + fRec108 + fRec116 + fTemp108);
			fRec104[0] = fRec107[1] + fRec115[1] + fRec127[1] + fRec135[1] + fRec108 + fRec116 + fTemp107 - (fRec111[1] + fRec119[1] + fRec123[1] + fRec131[1] + fRec112 + fRec120 + fTemp108);
			float fTemp109 = fRec136 + fRec124;
			float fTemp110 = fRec132 + fRec128;
			fRec105[0] = fRec107[1] + fRec119[1] + fRec123[1] + fRec135[1] + fRec108 + fRec120 + fTemp109 - (fRec111[1] + fRec115[1] + fRec127[1] + fRec131[1] + fRec112 + fRec116 + fTemp110);
			fRec106[0] = fRec111[1] + fRec115[1] + fRec123[1] + fRec135[1] + fRec112 + fRec116 + fTemp109 - (fRec107[1] + fRec119[1] + fRec127[1] + fRec131[1] + fRec108 + fRec120 + fTemp110);
			float fTemp111 = fRec140[1] + fRec144[1];
			float fTemp112 = fRec146[1] + fTemp111 + fRec142[1];
			float fTemp113 = fVec8[(IOTA0 - iConst226) & 8191];
			float fTemp114 = fVec8[(IOTA0 - iConst227) & 8191];
			float fTemp115 = fTemp113 + fTemp114;
			float fTemp116 = fVec8[(IOTA0 - iConst228) & 8191];
			float fTemp117 = fVec8[(IOTA0 - iConst229) & 8191];
			float fTemp118 = fTemp116 + fTemp117;
			float fTemp119 = fTemp115 + fTemp118;
			float fTemp120 = fVec8[(IOTA0 - iConst230) & 8191];
			float fTemp121 = fVec8[(IOTA0 - iConst231) & 8191];
			float fTemp122 = fTemp120 + fTemp121;
			float fTemp123 = fVec8[(IOTA0 - iConst232) & 8191];
			float fTemp124 = fVec8[(IOTA0 - iConst233) & 8191];
			float fTemp125 = fTemp123 + fTemp124;
			float fTemp126 = fTemp122 + fTemp125;
			fVec24[IOTA0 & 2047] = fTemp119 + fTemp126;
			float fTemp127 = fVec24[(IOTA0 - iConst234) & 2047];
			float fTemp128 = fTemp113 - fTemp114;
			float fTemp129 = fTemp116 - fTemp117;
			float fTemp130 = fTemp128 + fTemp129;
			float fTemp131 = fTemp120 - fTemp121;
			float fTemp132 = fTemp123 - fTemp124;
			float fTemp133 = fTemp131 + fTemp132;
			fVec25[IOTA0 & 4095] = fTemp130 + fTemp133;
			float fTemp134 = fVec25[(IOTA0 - iConst235) & 4095];
			float fTemp135 = fTemp127 - fTemp134;
			float fTemp136 = fTemp115 - fTemp118;
			float fTemp137 = fTemp122 - fTemp125;
			fVec26[IOTA0 & 4095] = fTemp136 + fTemp137;
			float fTemp138 = fVec26[(IOTA0 - iConst236) & 4095];
			float fTemp139 = fTemp128 - fTemp129;
			float fTemp140 = fTemp131 - fTemp132;
			fVec27[IOTA0 & 8191] = fTemp139 + fTemp140;
			float fTemp141 = fVec27[(IOTA0 - iConst237) & 8191];
			float fTemp142 = fTemp138 - fTemp141;
			float fTemp143 = fTemp135 + fTemp142;
			fVec28[IOTA0 & 4095] = fTemp119 - fTemp126;
			float fTemp144 = fVec28[(IOTA0 - iConst238) & 4095];
			fVec29[IOTA0 & 8191] = fTemp130 - fTemp133;
			float fTemp145 = fVec29[(IOTA0 - iConst239) & 8191];
			float fTemp146 = fTemp144 - fTemp145;
			fVec30[IOTA0 & 4095] = fTemp136 - fTemp137;
			float fTemp147 = fVec30[(IOTA0 - iConst240) & 4095];
			fVec31[IOTA0 & 8191] = fTemp139 - fTemp140;
			float fTemp148 = fVec31[(IOTA0 - iConst241) & 8191];
			float fTemp149 = fTemp147 - fTemp148;
			float fTemp150 = fTemp146 - fTemp149;
			fVec32[IOTA0 & 2047] = fTemp143 - fTemp150;
			float fTemp151 = fVec32[(IOTA0 - iConst243) & 2047];
			float fTemp152 = fTemp127 + fTemp134;
			float fTemp153 = fTemp138 + fTemp141;
			float fTemp154 = fTemp152 + fTemp153;
			float fTemp155 = fTemp144 + fTemp145;
			float fTemp156 = fTemp147 + fTemp148;
			float fTemp157 = fTemp155 - fTemp156;
			fVec33[IOTA0 & 4095] = fTemp154 - fTemp157;
			float fTemp158 = fVec33[(IOTA0 - iConst244) & 4095];
			float fTemp159 = fTemp151 - fTemp158;
			float fTemp160 = fTemp135 - fTemp142;
			float fTemp161 = fTemp146 + fTemp149;
			fVec34[IOTA0 & 4095] = fTemp160 - fTemp161;
			float fTemp162 = fVec34[(IOTA0 - iConst245) & 4095];
			float fTemp163 = fTemp152 - fTemp153;
			float fTemp164 = fTemp155 + fTemp156;
			fVec35[IOTA0 & 4095] = fTemp163 - fTemp164;
			float fTemp165 = fVec35[(IOTA0 - iConst246) & 4095];
			float fTemp166 = fTemp162 + fTemp165;
			float fTemp167 = fTemp159 - fTemp166;
			fVec36[IOTA0 & 4095] = fTemp143 + fTemp150;
			float fTemp168 = fVec36[(IOTA0 - iConst247) & 4095];
			fVec37[IOTA0 & 4095] = fTemp154 + fTemp157;
			float fTemp169 = fVec37[(IOTA0 - iConst248) & 4095];
			float fTemp170 = fTemp168 + fTemp169;
			fVec38[IOTA0 & 4095] = fTemp160 + fTemp161;
			float fTemp171 = fVec38[(IOTA0 - iConst249) & 4095];
			fVec39[IOTA0 & 8191] = fTemp163 + fTemp164;
			float fTemp172 = fVec39[(IOTA0 - iConst250) & 8191];
			float fTemp173 = fTemp171 + fTemp172;
			float fTemp174 = fTemp170 - fTemp173;
			fVec40[IOTA0 & 2047] = fTemp167 - fTemp174;
			float fTemp175 = 0.31335f * (fRec147[1] + fRec143[1] + fRec145[1] + fTemp112 + fRec141[1]) - 0.01f * fVec40[(IOTA0 - iConst234) & 2047];
			fVec41[IOTA0 & 16383] = fTemp175;
			fRec148[0] = -(fConst222 * (fConst223 * fRec148[1] - (fVec41[(IOTA0 - iConst252) & 16383] + fVec41[(IOTA0 - iConst253) & 16383])));
			float fTemp176 = fRec148[0] - fRec148[1];
			fRec140[0] = ((std::fabs(fTemp176) <= 0.001f) ? std::max<float>(-1.0f, std::min<float>(1.0f, 0.5f * (fRec148[0] + fRec148[1]))) : ((((fRec148[0] <= 1.0f) & (fRec148[0] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec148[0]) : fRec148[0] * static_cast<float>((fRec148[0] > 0.0f) - (fRec148[0] < 0.0f)) + -0.5f) - (((fRec148[1] <= 1.0f) & (fRec148[1] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec148[1]) : fRec148[1] * static_cast<float>((fRec148[1] > 0.0f) - (fRec148[1] < 0.0f)) + -0.5f)) / fTemp176);
			fVec42[IOTA0 & 4095] = fTemp167 + fTemp174;
			float fTemp177 = 0.31335f * (fTemp112 - (fRec147[1] + fRec141[1] + fRec145[1] + fRec143[1])) + 0.01f * fVec42[(IOTA0 - iConst247) & 4095];
			fVec43[IOTA0 & 16383] = fTemp177;
			fRec149[0] = -(fConst222 * (fConst223 * fRec149[1] - (fVec43[(IOTA0 - iConst254) & 16383] + fVec43[(IOTA0 - iConst255) & 16383])));
			float fTemp178 = fRec149[0] - fRec149[1];
			fRec141[0] = ((std::fabs(fTemp178) <= 0.001f) ? std::max<float>(-1.0f, std::min<float>(1.0f, 0.5f * (fRec149[0] + fRec149[1]))) : ((((fRec149[0] <= 1.0f) & (fRec149[0] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec149[0]) : fRec149[0] * static_cast<float>((fRec149[0] > 0.0f) - (fRec149[0] < 0.0f)) + -0.5f) - (((fRec149[1] <= 1.0f) & (fRec149[1] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec149[1]) : fRec149[1] * static_cast<float>((fRec149[1] > 0.0f) - (fRec149[1] < 0.0f)) + -0.5f)) / fTemp178);
			float fTemp179 = fRec142[1] + fRec146[1];
			float fTemp180 = fTemp159 + fTemp166;
			float fTemp181 = fTemp170 + fTemp173;
			fVec44[IOTA0 & 4095] = fTemp180 - fTemp181;
			float fTemp182 = 0.31335f * (fRec145[1] + fTemp111 + fRec141[1] - (fRec147[1] + fTemp179 + fRec143[1])) + 0.01f * fVec44[(IOTA0 - iConst256) & 4095];
			fVec45[IOTA0 & 16383] = fTemp182;
			fRec150[0] = -(fConst222 * (fConst223 * fRec150[1] - (fVec45[(IOTA0 - iConst257) & 16383] + fVec45[(IOTA0 - iConst258) & 16383])));
			float fTemp183 = fRec150[0] - fRec150[1];
			fRec142[0] = ((std::fabs(fTemp183) <= 0.001f) ? std::max<float>(-1.0f, std::min<float>(1.0f, 0.5f * (fRec150[0] + fRec150[1]))) : ((((fRec150[0] <= 1.0f) & (fRec150[0] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec150[0]) : fRec150[0] * static_cast<float>((fRec150[0] > 0.0f) - (fRec150[0] < 0.0f)) + -0.5f) - (((fRec150[1] <= 1.0f) & (fRec150[1] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec150[1]) : fRec150[1] * static_cast<float>((fRec150[1] > 0.0f) - (fRec150[1] < 0.0f)) + -0.5f)) / fTemp183);
			fVec46[IOTA0 & 4095] = fTemp180 + fTemp181;
			float fTemp184 = 0.31335f * (fRec147[1] + fTemp111 + fRec143[1] - (fRec145[1] + fTemp179 + fRec141[1])) - 0.01f * fVec46[(IOTA0 - iConst259) & 4095];
			fVec47[IOTA0 & 16383] = fTemp184;
			fRec151[0] = -(fConst222 * (fConst223 * fRec151[1] - (fVec47[(IOTA0 - iConst260) & 16383] + fVec47[(IOTA0 - iConst261) & 16383])));
			float fTemp185 = fRec151[0] - fRec151[1];
			fRec143[0] = ((std::fabs(fTemp185) <= 0.001f) ? std::max<float>(-1.0f, std::min<float>(1.0f, 0.5f * (fRec151[0] + fRec151[1]))) : ((((fRec151[0] <= 1.0f) & (fRec151[0] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec151[0]) : fRec151[0] * static_cast<float>((fRec151[0] > 0.0f) - (fRec151[0] < 0.0f)) + -0.5f) - (((fRec151[1] <= 1.0f) & (fRec151[1] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec151[1]) : fRec151[1] * static_cast<float>((fRec151[1] > 0.0f) - (fRec151[1] < 0.0f)) + -0.5f)) / fTemp185);
			float fTemp186 = fRec140[1] + fRec142[1];
			float fTemp187 = fRec144[1] + fRec146[1];
			float fTemp188 = fTemp151 + fTemp158;
			float fTemp189 = fTemp162 - fTemp165;
			float fTemp190 = fTemp188 - fTemp189;
			float fTemp191 = fTemp168 - fTemp169;
			float fTemp192 = fTemp171 - fTemp172;
			float fTemp193 = fTemp191 - fTemp192;
			fVec48[IOTA0 & 4095] = fTemp190 - fTemp193;
			float fTemp194 = 0.31335f * (fRec143[1] + fRec141[1] + fTemp186 - (fRec147[1] + fRec145[1] + fTemp187)) - 0.01f * fVec48[(IOTA0 - iConst262) & 4095];
			fVec49[IOTA0 & 16383] = fTemp194;
			fRec152[0] = -(fConst222 * (fConst223 * fRec152[1] - (fVec49[(IOTA0 - iConst263) & 16383] + fVec49[(IOTA0 - iConst264) & 16383])));
			float fTemp195 = fRec152[0] - fRec152[1];
			fRec144[0] = ((std::fabs(fTemp195) <= 0.001f) ? std::max<float>(-1.0f, std::min<float>(1.0f, 0.5f * (fRec152[0] + fRec152[1]))) : ((((fRec152[0] <= 1.0f) & (fRec152[0] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec152[0]) : fRec152[0] * static_cast<float>((fRec152[0] > 0.0f) - (fRec152[0] < 0.0f)) + -0.5f) - (((fRec152[1] <= 1.0f) & (fRec152[1] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec152[1]) : fRec152[1] * static_cast<float>((fRec152[1] > 0.0f) - (fRec152[1] < 0.0f)) + -0.5f)) / fTemp195);
			fVec50[IOTA0 & 8191] = fTemp190 + fTemp193;
			float fTemp196 = 0.31335f * (fRec147[1] + fRec145[1] + fTemp186 - (fRec143[1] + fRec141[1] + fTemp187)) - 0.01f * fVec50[(IOTA0 - iConst265) & 8191];
			fVec51[IOTA0 & 16383] = fTemp196;
			fRec153[0] = -(fConst222 * (fConst223 * fRec153[1] - (fVec51[(IOTA0 - iConst266) & 16383] + fVec51[(IOTA0 - iConst267) & 16383])));
			float fTemp197 = fRec153[0] - fRec153[1];
			fRec145[0] = ((std::fabs(fTemp197) <= 0.001f) ? std::max<float>(-1.0f, std::min<float>(1.0f, 0.5f * (fRec153[0] + fRec153[1]))) : ((((fRec153[0] <= 1.0f) & (fRec153[0] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec153[0]) : fRec153[0] * static_cast<float>((fRec153[0] > 0.0f) - (fRec153[0] < 0.0f)) + -0.5f) - (((fRec153[1] <= 1.0f) & (fRec153[1] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec153[1]) : fRec153[1] * static_cast<float>((fRec153[1] > 0.0f) - (fRec153[1] < 0.0f)) + -0.5f)) / fTemp197);
			float fTemp198 = fRec140[1] + fRec146[1];
			float fTemp199 = fRec144[1] + fRec142[1];
			float fTemp200 = fTemp188 + fTemp189;
			float fTemp201 = fTemp191 + fTemp192;
			fVec52[IOTA0 & 8191] = fTemp200 - fTemp201;
			float fTemp202 = 0.31335f * (fRec147[1] + fRec141[1] + fTemp198 - (fRec143[1] + fRec145[1] + fTemp199)) + 0.01f * fVec52[(IOTA0 - iConst268) & 8191];
			fVec53[IOTA0 & 16383] = fTemp202;
			fRec154[0] = -(fConst222 * (fConst223 * fRec154[1] - (fVec53[(IOTA0 - iConst269) & 16383] + fVec53[(IOTA0 - iConst270) & 16383])));
			float fTemp203 = fRec154[0] - fRec154[1];
			fRec146[0] = ((std::fabs(fTemp203) <= 0.001f) ? std::max<float>(-1.0f, std::min<float>(1.0f, 0.5f * (fRec154[0] + fRec154[1]))) : ((((fRec154[0] <= 1.0f) & (fRec154[0] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec154[0]) : fRec154[0] * static_cast<float>((fRec154[0] > 0.0f) - (fRec154[0] < 0.0f)) + -0.5f) - (((fRec154[1] <= 1.0f) & (fRec154[1] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec154[1]) : fRec154[1] * static_cast<float>((fRec154[1] > 0.0f) - (fRec154[1] < 0.0f)) + -0.5f)) / fTemp203);
			fVec54[IOTA0 & 8191] = fTemp200 + fTemp201;
			float fTemp204 = 0.31335f * (fRec143[1] + fRec145[1] + fTemp198 - (fRec147[1] + fRec141[1] + fTemp199)) - 0.01f * fVec54[(IOTA0 - iConst271) & 8191];
			fVec55[IOTA0 & 16383] = fTemp204;
			fRec155[0] = -(fConst222 * (fConst223 * fRec155[1] - (fVec55[(IOTA0 - iConst272) & 16383] + fVec55[(IOTA0 - iConst273) & 16383])));
			float fTemp205 = fRec155[0] - fRec155[1];
			fRec147[0] = ((std::fabs(fTemp205) <= 0.001f) ? std::max<float>(-1.0f, std::min<float>(1.0f, 0.5f * (fRec155[0] + fRec155[1]))) : ((((fRec155[0] <= 1.0f) & (fRec155[0] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec155[0]) : fRec155[0] * static_cast<float>((fRec155[0] > 0.0f) - (fRec155[0] < 0.0f)) + -0.5f) - (((fRec155[1] <= 1.0f) & (fRec155[1] >= -1.0f)) ? 0.5f * mydsp_faustpower2_f(fRec155[1]) : fRec155[1] * static_cast<float>((fRec155[1] > 0.0f) - (fRec155[1] < 0.0f)) + -0.5f)) / fTemp205);
			float fTemp206 = fRec140[0] + fRec141[0] + fRec142[0] + fRec143[0] + fRec144[0] + fRec145[0] + fRec146[0] + fRec147[0];
			fVec56[0] = fTemp206;
			fRec139[0] = -(fConst219 * (fConst220 * fRec139[1] - fConst218 * (fTemp206 - fVec56[1])));
			fRec165[IOTA0 & 2047] = -(fConst24 * (fConst25 * fRec165[(IOTA0 - 1) & 2047] - fConst21 * (fTemp95 - fVec8[(IOTA0 - 1) & 8191])));
			float fTemp207 = 2.0f * fRec165[(IOTA0 - iConst274) & 2047];
			fRec166[0] = 0.5f * (fRec166[1] + fRec164[1]);
			fVec57[IOTA0 & 8191] = fTemp207 + 0.8f * fRec166[0];
			fRec164[0] = fVec57[(IOTA0 - iConst276) & 8191];
			fRec168[0] = 0.5f * (fRec168[1] + fRec167[1]);
			fVec58[IOTA0 & 8191] = fTemp207 + 0.8f * fRec168[0];
			fRec167[0] = fVec58[(IOTA0 - iConst278) & 8191];
			fRec170[0] = 0.5f * (fRec170[1] + fRec169[1]);
			fVec59[IOTA0 & 8191] = fTemp207 + 0.8f * fRec170[0];
			fRec169[0] = fVec59[(IOTA0 - iConst280) & 8191];
			fRec172[0] = 0.5f * (fRec172[1] + fRec171[1]);
			fVec60[IOTA0 & 8191] = fTemp207 + 0.8f * fRec172[0];
			fRec171[0] = fVec60[(IOTA0 - iConst282) & 8191];
			fRec174[0] = 0.5f * (fRec174[1] + fRec173[1]);
			fVec61[IOTA0 & 8191] = fTemp207 + 0.8f * fRec174[0];
			fRec173[0] = fVec61[(IOTA0 - iConst284) & 8191];
			fRec176[0] = 0.5f * (fRec176[1] + fRec175[1]);
			fVec62[IOTA0 & 8191] = fTemp207 + 0.8f * fRec176[0];
			fRec175[0] = fVec62[(IOTA0 - iConst286) & 8191];
			fRec178[0] = 0.5f * (fRec178[1] + fRec177[1]);
			fVec63[IOTA0 & 8191] = fTemp207 + 0.8f * fRec178[0];
			fRec177[0] = fVec63[(IOTA0 - iConst288) & 8191];
			fRec180[0] = 0.5f * (fRec180[1] + fRec179[1]);
			fVec64[IOTA0 & 8191] = fTemp207 + 0.8f * fRec180[0];
			fRec179[0] = fVec64[(IOTA0 - iConst290) & 8191];
			float fTemp208 = fRec164[1] + fRec167[1] + fRec169[1] + fRec171[1] + fRec173[1] + fRec175[1] + fRec177[1] + 0.5f * fRec162[1] + fRec179[1];
			fVec65[IOTA0 & 2047] = fTemp208;
			fRec162[0] = fVec65[(IOTA0 - iConst292) & 2047];
			float fRec163 = -(0.5f * fTemp208);
			float fTemp209 = fRec162[1] + fRec163 + 0.5f * fRec160[1];
			fVec66[IOTA0 & 2047] = fTemp209;
			fRec160[0] = fVec66[(IOTA0 - iConst294) & 2047];
			float fRec161 = -(0.5f * fTemp209);
			float fTemp210 = fRec160[1] + fRec161 + 0.5f * fRec158[1];
			fVec67[IOTA0 & 2047] = fTemp210;
			fRec158[0] = fVec67[(IOTA0 - iConst296) & 2047];
			float fRec159 = -(0.5f * fTemp210);
			float fTemp211 = fRec158[1] + fRec159 + 0.5f * fRec156[1];
			fVec68[IOTA0 & 1023] = fTemp211;
			fRec156[0] = fVec68[(IOTA0 - iConst298) & 1023];
			float fRec157 = -(0.5f * fTemp211);
			fRec190[0] = 0.5f * (fRec190[1] + fRec189[1]);
			fVec69[IOTA0 & 8191] = fTemp207 + 0.8f * fRec190[0];
			fRec189[0] = fVec69[(IOTA0 - iConst299) & 8191];
			fRec192[0] = 0.5f * (fRec192[1] + fRec191[1]);
			fVec70[IOTA0 & 8191] = fTemp207 + 0.8f * fRec192[0];
			fRec191[0] = fVec70[(IOTA0 - iConst300) & 8191];
			fRec194[0] = 0.5f * (fRec194[1] + fRec193[1]);
			fVec71[IOTA0 & 8191] = fTemp207 + 0.8f * fRec194[0];
			fRec193[0] = fVec71[(IOTA0 - iConst301) & 8191];
			fRec196[0] = 0.5f * (fRec196[1] + fRec195[1]);
			fVec72[IOTA0 & 8191] = fTemp207 + 0.8f * fRec196[0];
			fRec195[0] = fVec72[(IOTA0 - iConst302) & 8191];
			fRec198[0] = 0.5f * (fRec198[1] + fRec197[1]);
			fVec73[IOTA0 & 8191] = fTemp207 + 0.8f * fRec198[0];
			fRec197[0] = fVec73[(IOTA0 - iConst303) & 8191];
			fRec200[0] = 0.5f * (fRec200[1] + fRec199[1]);
			fVec74[IOTA0 & 8191] = fTemp207 + 0.8f * fRec200[0];
			fRec199[0] = fVec74[(IOTA0 - iConst304) & 8191];
			fRec202[0] = 0.5f * (fRec202[1] + fRec201[1]);
			fVec75[IOTA0 & 8191] = fTemp207 + 0.8f * fRec202[0];
			fRec201[0] = fVec75[(IOTA0 - iConst305) & 8191];
			fRec204[0] = 0.5f * (fRec204[1] + fRec203[1]);
			fVec76[IOTA0 & 8191] = 0.8f * fRec204[0] + fTemp207;
			fRec203[0] = fVec76[(IOTA0 - iConst306) & 8191];
			float fTemp212 = fRec189[1] + fRec191[1] + fRec193[1] + fRec195[1] + fRec197[1] + fRec199[1] + fRec201[1] + 0.5f * fRec187[1] + fRec203[1];
			fVec77[IOTA0 & 2047] = fTemp212;
			fRec187[0] = fVec77[(IOTA0 - iConst307) & 2047];
			float fRec188 = -(0.5f * fTemp212);
			float fTemp213 = fRec187[1] + fRec188 + 0.5f * fRec185[1];
			fVec78[IOTA0 & 2047] = fTemp213;
			fRec185[0] = fVec78[(IOTA0 - iConst308) & 2047];
			float fRec186 = -(0.5f * fTemp213);
			float fTemp214 = fRec185[1] + fRec186 + 0.5f * fRec183[1];
			fVec79[IOTA0 & 2047] = fTemp214;
			fRec183[0] = fVec79[(IOTA0 - iConst309) & 2047];
			float fRec184 = -(0.5f * fTemp214);
			float fTemp215 = fRec183[1] + fRec184 + 0.5f * fRec181[1];
			fVec80[IOTA0 & 1023] = fTemp215;
			fRec181[0] = fVec80[(IOTA0 - iConst310) & 1023];
			float fRec182 = -(0.5f * fTemp215);
			float fTemp216 = fRec2[0] * (1.0f - fRec97[0]) * fTemp93 + ((iSlow153) ? 0.5f * fRec97[0] * (fRec156[1] + fRec181[1] + fRec182 + fRec157) : ((iSlow154) ? fRec97[0] * (fTemp95 + 0.19024675f * fRec139[0]) : 0.185f * fRec97[0] * (fRec100[0] + fRec101[0] + (fRec100[0] - fRec101[0]))));
			float fTemp217 = fSlow159 * fTemp216;
			fVec81[IOTA0 & 1023] = fTemp217;
			float fTemp218 = fSlow158 * fTemp216;
			fVec82[IOTA0 & 1023] = fTemp218;
			float fTemp219 = fSlow157 * fTemp216;
			fVec83[IOTA0 & 1023] = fTemp219;
			float fTemp220 = ((iSlow0) ? fTemp216 : fRec0[0] * ((1.0f - fRec1[0]) * fTemp216 + fRec1[0] * fSlow156 * (fSlow160 * fTemp216 + 0.0006539f * fVec81[(IOTA0 - 615) & 1023] + 0.00173005f * fVec81[(IOTA0 - 614) & 1023] + 0.0019844f * fVec81[(IOTA0 - 613) & 1023] + 0.00123465f * fVec81[(IOTA0 - 612) & 1023] + 0.00053127f * fVec81[(IOTA0 - 611) & 1023] + 0.00075599f * fVec81[(IOTA0 - 604) & 1023] + 0.00087297f * fVec81[(IOTA0 - 603) & 1023] + 0.00062689f * fVec81[(IOTA0 - 602) & 1023] + 0.00030887f * fVec81[(IOTA0 - 601) & 1023] + 0.00058291f * fVec81[(IOTA0 - 580) & 1023] + 0.00537296f * fVec81[(IOTA0 - 579) & 1023] + 0.01042282f * fVec81[(IOTA0 - 578) & 1023] + 0.01334378f * fVec81[(IOTA0 - 577) & 1023] + 0.01362419f * fVec81[(IOTA0 - 576) & 1023] + 0.01110634f * fVec81[(IOTA0 - 575) & 1023] + 0.00623588f * fVec81[(IOTA0 - 574) & 1023] + 0.00066322f * fVec81[(IOTA0 - 573) & 1023] + 0.00160847f * fVec81[(IOTA0 - 528) & 1023] + 0.00269433f * fVec81[(IOTA0 - 527) & 1023] + 0.00248124f * fVec81[(IOTA0 - 526) & 1023] + 0.00124178f * fVec81[(IOTA0 - 525) & 1023] + 0.001149f * fVec81[(IOTA0 - 460) & 1023] + 0.00341559f * fVec81[(IOTA0 - 459) & 1023] + 0.00530165f * fVec81[(IOTA0 - 458) & 1023] + 0.00599522f * fVec81[(IOTA0 - 457) & 1023] + 0.00467882f * fVec81[(IOTA0 - 456) & 1023] + 0.00166353f * fVec81[(IOTA0 - 455) & 1023] + 0.00196121f * fVec81[(IOTA0 - 445) & 1023] + 0.00463352f * fVec81[(IOTA0 - 444) & 1023] + 0.00582967f * fVec81[(IOTA0 - 443) & 1023] + 0.00554303f * fVec81[(IOTA0 - 442) & 1023] + 0.00412916f * fVec81[(IOTA0 - 441) & 1023] + 0.00181537f * fVec81[(IOTA0 - 440) & 1023] + 0.00142142f * fVec81[(IOTA0 - 424) & 1023] + 0.0052293f * fVec81[(IOTA0 - 423) & 1023] + 0.00892419f * fVec81[(IOTA0 - 422) & 1023] + 0.01134605f * fVec81[(IOTA0 - 421) & 1023] + 0.01165376f * fVec81[(IOTA0 - 420) & 1023] + 0.00949453f * fVec81[(IOTA0 - 419) & 1023] + 0.00543875f * fVec81[(IOTA0 - 418) & 1023] + 0.00077831f * fVec81[(IOTA0 - 417) & 1023] + 0.00041185f * fVec81[(IOTA0 - 386) & 1023] + 0.00391002f * fVec81[(IOTA0 - 385) & 1023] + 0.00767046f * fVec81[(IOTA0 - 384) & 1023] + 0.01146706f * fVec81[(IOTA0 - 383) & 1023] + 0.01529885f * fVec81[(IOTA0 - 382) & 1023] + 0.01909827f * fVec81[(IOTA0 - 381) & 1023] + 0.02229681f * fVec81[(IOTA0 - 380) & 1023] + 0.02449103f * fVec81[(IOTA0 - 379) & 1023] + 0.02544733f * fVec81[(IOTA0 - 378) & 1023] + 0.02558605f * fVec81[(IOTA0 - 377) & 1023] + 0.02520837f * fVec81[(IOTA0 - 376) & 1023] + 0.02450496f * fVec81[(IOTA0 - 375) & 1023] + 0.02379359f * fVec81[(IOTA0 - 374) & 1023] + 0.02325906f * fVec81[(IOTA0 - 373) & 1023] + 0.02318197f * fVec81[(IOTA0 - 372) & 1023] + 0.02383445f * fVec81[(IOTA0 - 371) & 1023] + 0.02519187f * fVec81[(IOTA0 - 370) & 1023] + 0.02711624f * fVec81[(IOTA0 - 369) & 1023] + 0.02941138f * fVec81[(IOTA0 - 368) & 1023] + 0.0311828f * fVec81[(IOTA0 - 367) & 1023] + 0.0320365f * fVec81[(IOTA0 - 366) & 1023] + 0.03183996f * fVec81[(IOTA0 - 365) & 1023] + 0.03084273f * fVec81[(IOTA0 - 364) & 1023] + 0.02965108f * fVec81[(IOTA0 - 363) & 1023] + 0.02906979f * fVec81[(IOTA0 - 362) & 1023] + 0.02888862f * fVec81[(IOTA0 - 361) & 1023] + 0.02893389f * fVec81[(IOTA0 - 360) & 1023] + 0.02857353f * fVec81[(IOTA0 - 359) & 1023] + 0.02702016f * fVec81[(IOTA0 - 358) & 1023] + 0.02410183f * fVec81[(IOTA0 - 357) & 1023] + 0.02046423f * fVec81[(IOTA0 - 356) & 1023] + 0.01669285f * fVec81[(IOTA0 - 355) & 1023] + 0.01366872f * fVec81[(IOTA0 - 354) & 1023] + 0.01194384f * fVec81[(IOTA0 - 353) & 1023] + 0.01161827f * fVec81[(IOTA0 - 352) & 1023] + 0.01238166f * fVec81[(IOTA0 - 351) & 1023] + 0.01361873f * fVec81[(IOTA0 - 350) & 1023] + 0.01462602f * fVec81[(IOTA0 - 349) & 1023] + 0.01509844f * fVec81[(IOTA0 - 348) & 1023] + 0.0148662f * fVec81[(IOTA0 - 347) & 1023] + 0.01421677f * fVec81[(IOTA0 - 346) & 1023] + 0.01334597f * fVec81[(IOTA0 - 345) & 1023] + 0.01292565f * fVec81[(IOTA0 - 344) & 1023] + 0.01345019f * fVec81[(IOTA0 - 343) & 1023] + 0.01518638f * fVec81[(IOTA0 - 342) & 1023] + 0.01768924f * fVec81[(IOTA0 - 341) & 1023] + 0.0203399f * fVec81[(IOTA0 - 340) & 1023] + 0.02232736f * fVec81[(IOTA0 - 339) & 1023] + 0.02327565f * fVec81[(IOTA0 - 338) & 1023] + 0.02294824f * fVec81[(IOTA0 - 337) & 1023] + 0.02118557f * fVec81[(IOTA0 - 336) & 1023] + 0.0185808f * fVec81[(IOTA0 - 335) & 1023] + 0.01579172f * fVec81[(IOTA0 - 334) & 1023] + 0.01329824f * fVec81[(IOTA0 - 333) & 1023] + 0.01147643f * fVec81[(IOTA0 - 332) & 1023] + 0.01070453f * fVec81[(IOTA0 - 331) & 1023] + 0.01113708f * fVec81[(IOTA0 - 330) & 1023] + 0.01222108f * fVec81[(IOTA0 - 329) & 1023] + 0.01314657f * fVec81[(IOTA0 - 328) & 1023] + 0.0135849f * fVec81[(IOTA0 - 327) & 1023] + 0.0136185f * fVec81[(IOTA0 - 326) & 1023] + 0.01316085f * fVec81[(IOTA0 - 325) & 1023] + 0.0126329f * fVec81[(IOTA0 - 324) & 1023] + 0.01246858f * fVec81[(IOTA0 - 323) & 1023] + 0.0132772f * fVec81[(IOTA0 - 322) & 1023] + 0.01485783f * fVec81[(IOTA0 - 321) & 1023] + 0.01649485f * fVec81[(IOTA0 - 320) & 1023] + 0.01792411f * fVec81[(IOTA0 - 319) & 1023] + 0.01936204f * fVec81[(IOTA0 - 318) & 1023] + 0.01994089f * fVec81[(IOTA0 - 317) & 1023] + 0.01911706f * fVec81[(IOTA0 - 316) & 1023] + 0.01771721f * fVec81[(IOTA0 - 315) & 1023] + 0.01706408f * fVec81[(IOTA0 - 314) & 1023] + 0.01819456f * fVec81[(IOTA0 - 313) & 1023] + 0.02161935f * fVec81[(IOTA0 - 312) & 1023] + 0.02669354f * fVec81[(IOTA0 - 311) & 1023] + 0.03185412f * fVec81[(IOTA0 - 310) & 1023] + 0.0349579f * fVec81[(IOTA0 - 309) & 1023] + 0.03448853f * fVec81[(IOTA0 - 308) & 1023] + 0.0306907f * fVec81[(IOTA0 - 307) & 1023] + 0.0256495f * fVec81[(IOTA0 - 306) & 1023] + 0.02163248f * fVec81[(IOTA0 - 305) & 1023] + 0.02020883f * fVec81[(IOTA0 - 304) & 1023] + 0.0218408f * fVec81[(IOTA0 - 303) & 1023] + 0.0258105f * fVec81[(IOTA0 - 302) & 1023] + 0.030796f * fVec81[(IOTA0 - 301) & 1023] + 0.0357332f * fVec81[(IOTA0 - 300) & 1023] + 0.03885973f * fVec81[(IOTA0 - 299) & 1023] + 0.03952881f * fVec81[(IOTA0 - 298) & 1023] + 0.03833907f * fVec81[(IOTA0 - 297) & 1023] + 0.03651549f * fVec81[(IOTA0 - 296) & 1023] + 0.03481144f * fVec81[(IOTA0 - 295) & 1023] + 0.0344566f * fVec81[(IOTA0 - 294) & 1023] + 0.03556516f * fVec81[(IOTA0 - 293) & 1023] + 0.03805011f * fVec81[(IOTA0 - 292) & 1023] + 0.04106877f * fVec81[(IOTA0 - 291) & 1023] + 0.04393765f * fVec81[(IOTA0 - 290) & 1023] + 0.04593129f * fVec81[(IOTA0 - 289) & 1023] + 0.04709581f * fVec81[(IOTA0 - 288) & 1023] + 0.04793662f * fVec81[(IOTA0 - 287) & 1023] + 0.04879188f * fVec81[(IOTA0 - 286) & 1023] + 0.0494025f * fVec81[(IOTA0 - 285) & 1023] + 0.0497574f * fVec81[(IOTA0 - 284) & 1023] + 0.05000941f * fVec81[(IOTA0 - 283) & 1023] + 0.05009688f * fVec81[(IOTA0 - 282) & 1023] + 0.04990563f * fVec81[(IOTA0 - 281) & 1023] + 0.04955136f * fVec81[(IOTA0 - 280) & 1023] + 0.04884137f * fVec81[(IOTA0 - 279) & 1023] + 0.04752695f * fVec81[(IOTA0 - 278) & 1023] + 0.04603525f * fVec81[(IOTA0 - 277) & 1023] + 0.04443801f * fVec81[(IOTA0 - 276) & 1023] + 0.04272159f * fVec81[(IOTA0 - 275) & 1023] + 0.04129378f * fVec81[(IOTA0 - 274) & 1023] + 0.03974336f * fVec81[(IOTA0 - 273) & 1023] + 0.03817518f * fVec81[(IOTA0 - 272) & 1023] + 0.03666509f * fVec81[(IOTA0 - 271) & 1023] + 0.03589951f * fVec81[(IOTA0 - 270) & 1023] + 0.03588865f * fVec81[(IOTA0 - 269) & 1023] + 0.03636971f * fVec81[(IOTA0 - 268) & 1023] + 0.03578246f * fVec81[(IOTA0 - 267) & 1023] + 0.03351505f * fVec81[(IOTA0 - 266) & 1023] + 0.02871812f * fVec81[(IOTA0 - 265) & 1023] + 0.02126926f * fVec81[(IOTA0 - 264) & 1023] + 0.0121991f * fVec81[(IOTA0 - 263) & 1023] + 0.00357137f * fVec81[(IOTA0 - 262) & 1023] + 0.00377031f * fVec81[(IOTA0 - 249) & 1023] + 0.00800484f * fVec81[(IOTA0 - 248) & 1023] + 0.01096652f * fVec81[(IOTA0 - 247) & 1023] + 0.01291671f * fVec81[(IOTA0 - 246) & 1023] + 0.01385369f * fVec81[(IOTA0 - 245) & 1023] + 0.01469245f * fVec81[(IOTA0 - 244) & 1023] + 0.01568653f * fVec81[(IOTA0 - 243) & 1023] + 0.01715255f * fVec81[(IOTA0 - 242) & 1023] + 0.01893945f * fVec81[(IOTA0 - 241) & 1023] + 0.02132854f * fVec81[(IOTA0 - 240) & 1023] + 0.02293781f * fVec81[(IOTA0 - 239) & 1023] + 0.02304744f * fVec81[(IOTA0 - 238) & 1023] + 0.02221572f * fVec81[(IOTA0 - 237) & 1023] + 0.02158404f * fVec81[(IOTA0 - 236) & 1023] + 0.02163449f * fVec81[(IOTA0 - 235) & 1023] + 0.02254789f * fVec81[(IOTA0 - 234) & 1023] + 0.02445381f * fVec81[(IOTA0 - 233) & 1023] + 0.02639876f * fVec81[(IOTA0 - 232) & 1023] + 0.02819093f * fVec81[(IOTA0 - 231) & 1023] + 0.02981789f * fVec81[(IOTA0 - 230) & 1023] + 0.03137662f * fVec81[(IOTA0 - 229) & 1023] + 0.03204964f * fVec81[(IOTA0 - 228) & 1023] + 0.03322425f * fVec81[(IOTA0 - 227) & 1023] + 0.03511969f * fVec81[(IOTA0 - 226) & 1023] + 0.03870797f * fVec81[(IOTA0 - 225) & 1023] + 0.04334234f * fVec81[(IOTA0 - 224) & 1023] + 0.04833131f * fVec81[(IOTA0 - 223) & 1023] + 0.05189192f * fVec81[(IOTA0 - 222) & 1023] + 0.05309498f * fVec81[(IOTA0 - 221) & 1023] + 0.04977712f * fVec81[(IOTA0 - 220) & 1023] + 0.04267233f * fVec81[(IOTA0 - 219) & 1023] + 0.03302754f * fVec81[(IOTA0 - 218) & 1023] + 0.02351972f * fVec81[(IOTA0 - 217) & 1023] + 0.01626799f * fVec81[(IOTA0 - 216) & 1023] + 0.01234327f * fVec81[(IOTA0 - 215) & 1023] + 0.0109475f * fVec81[(IOTA0 - 214) & 1023] + 0.01111421f * fVec81[(IOTA0 - 213) & 1023] + 0.01188585f * fVec81[(IOTA0 - 212) & 1023] + 0.01322663f * fVec81[(IOTA0 - 211) & 1023] + 0.01495951f * fVec81[(IOTA0 - 210) & 1023] + 0.0182434f * fVec81[(IOTA0 - 209) & 1023] + 0.02330978f * fVec81[(IOTA0 - 208) & 1023] + 0.02947317f * fVec81[(IOTA0 - 207) & 1023] + 0.0331992f * fVec81[(IOTA0 - 206) & 1023] + 0.03340296f * fVec81[(IOTA0 - 205) & 1023] + 0.02841482f * fVec81[(IOTA0 - 204) & 1023] + 0.02028084f * fVec81[(IOTA0 - 203) & 1023] + 0.01004501f * fVec81[(IOTA0 - 202) & 1023] + 0.00199851f * fVec81[(IOTA0 - 201) & 1023] + 0.00637589f * fVec81[(IOTA0 - 198) & 1023] + 0.01600816f * fVec81[(IOTA0 - 197) & 1023] + 0.02494395f * fVec81[(IOTA0 - 196) & 1023] + 0.0327389f * fVec81[(IOTA0 - 195) & 1023] + 0.03517488f * fVec81[(IOTA0 - 194) & 1023] + 0.0325508f * fVec81[(IOTA0 - 193) & 1023] + 0.02558564f * fVec81[(IOTA0 - 192) & 1023] + 0.01722327f * fVec81[(IOTA0 - 191) & 1023] + 0.00896532f * fVec81[(IOTA0 - 190) & 1023] + 0.00299114f * fVec81[(IOTA0 - 189) & 1023] + 0.01682599f * fVec81[(IOTA0 - 137) & 1023] + 0.02676155f * fVec81[(IOTA0 - 136) & 1023] + 0.02850401f * fVec81[(IOTA0 - 135) & 1023] + 0.01440594f * fVec81[(IOTA0 - 134) & 1023] + 0.00579957f * fVec81[(IOTA0 - 124) & 1023] + 0.01101614f * fVec81[(IOTA0 - 123) & 1023] + 0.01021953f * fVec81[(IOTA0 - 122) & 1023] + 0.01261901f * fVec81[(IOTA0 - 111) & 1023] + 0.01554558f * fVec81[(IOTA0 - 110) & 1023] + 0.00016029f * fVec81[(IOTA0 - 90) & 1023] + 0.0171676f * fVec81[(IOTA0 - 89) & 1023] + 0.03002375f * fVec81[(IOTA0 - 88) & 1023] + 0.0246181f * fVec81[(IOTA0 - 87) & 1023] + 0.0081661f * fVec81[(IOTA0 - 86) & 1023] + 0.01566794f * fVec81[(IOTA0 - 73) & 1023] + 0.03150516f * fVec81[(IOTA0 - 72) & 1023] + 0.01847036f * fVec81[(IOTA0 - 71) & 1023] + 0.03536055f * fVec81[(IOTA0 - 63) & 1023] + 0.05746161f * fVec81[(IOTA0 - 62) & 1023] + 0.05018681f * fVec81[(IOTA0 - 61) & 1023] + 0.01781079f * fVec81[(IOTA0 - 60) & 1023] + 0.04211542f * fVec81[(IOTA0 - 47) & 1023] + 0.09706095f * fVec81[(IOTA0 - 46) & 1023] + 0.11317102f * fVec81[(IOTA0 - 45) & 1023] + 0.09529059f * fVec81[(IOTA0 - 44) & 1023] + 0.06064126f * fVec81[(IOTA0 - 43) & 1023] + 0.01564805f * fVec81[(IOTA0 - 42) & 1023] + 0.0033579f * fVec81[(IOTA0 - 32) & 1023] + 0.08136796f * fVec81[(IOTA0 - 31) & 1023] + 0.10099909f * fVec81[(IOTA0 - 30) & 1023] + 0.08615623f * fVec81[(IOTA0 - 29) & 1023] + 0.02594566f * fVec81[(IOTA0 - 28) & 1023] + 0.00277334f * fVec81[(IOTA0 - 25) & 1023] + 0.05937026f * fVec81[(IOTA0 - 24) & 1023] + 0.12404779f * fVec81[(IOTA0 - 23) & 1023] + 0.14002986f * fVec81[(IOTA0 - 22) & 1023] + 0.09891198f * fVec81[(IOTA0 - 21) & 1023] + 0.03712848f * fVec81[(IOTA0 - 20) & 1023] + 0.006514f * fVec81[(IOTA0 - 17) & 1023] + 0.08023275f * fVec81[(IOTA0 - 16) & 1023] + 0.15491265f * fVec81[(IOTA0 - 15) & 1023] + 0.19446647f * fVec81[(IOTA0 - 14) & 1023] + 0.09261103f * fVec81[(IOTA0 - 13) & 1023] + 0.07622134f * fVec81[(IOTA0 - 7) & 1023] + 0.46983454f * fVec81[(IOTA0 - 6) & 1023] + 0.8391074f * fVec81[(IOTA0 - 5) & 1023] + fVec81[(IOTA0 - 4) & 1023] + 0.93681824f * fVec81[(IOTA0 - 3) & 1023] + 0.70795596f * fVec81[(IOTA0 - 2) & 1023] + 0.3864263f * fVec81[(IOTA0 - 1) & 1023] + 0.01134821f * (fVec82[(IOTA0 - 516) & 1023] - fVec83[(IOTA0 - 332) & 1023]) + 0.0278191f * fVec83[(IOTA0 - 639) & 1023] + 0.02570384f * fVec83[(IOTA0 - 638) & 1023] + 0.02328172f * fVec83[(IOTA0 - 637) & 1023] + 0.02085861f * fVec83[(IOTA0 - 636) & 1023] + 0.01867711f * fVec83[(IOTA0 - 635) & 1023] + 0.01695843f * fVec83[(IOTA0 - 634) & 1023] + 0.01584036f * fVec83[(IOTA0 - 633) & 1023] + 0.01534269f * fVec83[(IOTA0 - 632) & 1023] + 0.01540992f * fVec83[(IOTA0 - 631) & 1023] + 0.0160334f * fVec83[(IOTA0 - 630) & 1023] + 0.01726257f * fVec83[(IOTA0 - 629) & 1023] + 0.019036f * fVec83[(IOTA0 - 628) & 1023] + 0.02109873f * fVec83[(IOTA0 - 627) & 1023] + 0.0231354f * fVec83[(IOTA0 - 626) & 1023] + 0.02500127f * fVec83[(IOTA0 - 625) & 1023] + 0.02667867f * fVec83[(IOTA0 - 624) & 1023] + 0.0281723f * fVec83[(IOTA0 - 623) & 1023] + 0.0294315f * fVec83[(IOTA0 - 622) & 1023] + 0.03039682f * fVec83[(IOTA0 - 621) & 1023] + 0.03104501f * fVec83[(IOTA0 - 620) & 1023] + 0.03139846f * fVec83[(IOTA0 - 619) & 1023] + 0.03153502f * fVec83[(IOTA0 - 618) & 1023] + 0.03157f * fVec83[(IOTA0 - 617) & 1023] + 0.03165057f * fVec83[(IOTA0 - 616) & 1023] + 0.03182445f * fVec83[(IOTA0 - 615) & 1023] + 0.03206013f * fVec83[(IOTA0 - 614) & 1023] + 0.0323251f * fVec83[(IOTA0 - 613) & 1023] + 0.03267138f * fVec83[(IOTA0 - 612) & 1023] + 0.03308365f * fVec83[(IOTA0 - 611) & 1023] + 0.03338792f * fVec83[(IOTA0 - 610) & 1023] + 0.03333008f * fVec83[(IOTA0 - 609) & 1023] + 0.03275813f * fVec83[(IOTA0 - 608) & 1023] + 0.03172547f * fVec83[(IOTA0 - 607) & 1023] + 0.03038508f * fVec83[(IOTA0 - 606) & 1023] + 0.02891541f * fVec83[(IOTA0 - 605) & 1023] + 0.02745589f * fVec83[(IOTA0 - 604) & 1023] + 0.02612094f * fVec83[(IOTA0 - 603) & 1023] + 0.02500522f * fVec83[(IOTA0 - 602) & 1023] + 0.02422899f * fVec83[(IOTA0 - 601) & 1023] + 0.02404016f * fVec83[(IOTA0 - 600) & 1023] + 0.02471653f * fVec83[(IOTA0 - 599) & 1023] + 0.02636403f * fVec83[(IOTA0 - 598) & 1023] + 0.02887093f * fVec83[(IOTA0 - 597) & 1023] + 0.03204703f * fVec83[(IOTA0 - 596) & 1023] + 0.03568213f * fVec83[(IOTA0 - 595) & 1023] + 0.0395152f * fVec83[(IOTA0 - 594) & 1023] + 0.04318985f * fVec83[(IOTA0 - 593) & 1023] + 0.04636855f * fVec83[(IOTA0 - 592) & 1023] + 0.04885036f * fVec83[(IOTA0 - 591) & 1023] + 0.05054717f * fVec83[(IOTA0 - 590) & 1023] + 0.05145477f * fVec83[(IOTA0 - 589) & 1023] + 0.05169724f * fVec83[(IOTA0 - 588) & 1023] + 0.05151038f * fVec83[(IOTA0 - 587) & 1023] + 0.05106894f * fVec83[(IOTA0 - 586) & 1023] + 0.05044348f * fVec83[(IOTA0 - 585) & 1023] + 0.04969889f * fVec83[(IOTA0 - 584) & 1023] + 0.04903215f * fVec83[(IOTA0 - 583) & 1023] + 0.04864101f * fVec83[(IOTA0 - 582) & 1023] + 0.04854993f * fVec83[(IOTA0 - 581) & 1023] + 0.04864867f * fVec83[(IOTA0 - 580) & 1023] + 0.04883998f * fVec83[(IOTA0 - 579) & 1023] + 0.04905885f * fVec83[(IOTA0 - 578) & 1023] + 0.0491524f * fVec83[(IOTA0 - 577) & 1023] + 0.04890783f * fVec83[(IOTA0 - 576) & 1023] + 0.04820871f * fVec83[(IOTA0 - 575) & 1023] + 0.04707953f * fVec83[(IOTA0 - 574) & 1023] + 0.04560851f * fVec83[(IOTA0 - 573) & 1023] + 0.04395409f * fVec83[(IOTA0 - 572) & 1023] + 0.04236937f * fVec83[(IOTA0 - 571) & 1023] + 0.04115282f * fVec83[(IOTA0 - 570) & 1023] + 0.04046519f * fVec83[(IOTA0 - 569) & 1023] + 0.04027549f * fVec83[(IOTA0 - 568) & 1023] + 0.0405323f * fVec83[(IOTA0 - 567) & 1023] + 0.04130297f * fVec83[(IOTA0 - 566) & 1023] + 0.04265139f * fVec83[(IOTA0 - 565) & 1023] + 0.04447524f * fVec83[(IOTA0 - 564) & 1023] + 0.04655232f * fVec83[(IOTA0 - 563) & 1023] + 0.04871504f * fVec83[(IOTA0 - 562) & 1023] + 0.0508476f * fVec83[(IOTA0 - 561) & 1023] + 0.05282729f * fVec83[(IOTA0 - 560) & 1023] + 0.05454288f * fVec83[(IOTA0 - 559) & 1023] + 0.05591652f * fVec83[(IOTA0 - 558) & 1023] + 0.05688047f * fVec83[(IOTA0 - 557) & 1023] + 0.05734589f * fVec83[(IOTA0 - 556) & 1023] + 0.05727149f * fVec83[(IOTA0 - 555) & 1023] + 0.05674255f * fVec83[(IOTA0 - 554) & 1023] + 0.05592986f * fVec83[(IOTA0 - 553) & 1023] + 0.05486383f * fVec83[(IOTA0 - 552) & 1023] + 0.05347796f * fVec83[(IOTA0 - 551) & 1023] + 0.05176249f * fVec83[(IOTA0 - 550) & 1023] + 0.0498546f * fVec83[(IOTA0 - 549) & 1023] + 0.04791854f * fVec83[(IOTA0 - 548) & 1023] + 0.04598902f * fVec83[(IOTA0 - 547) & 1023] + 0.04406223f * fVec83[(IOTA0 - 546) & 1023] + 0.04219426f * fVec83[(IOTA0 - 545) & 1023] + 0.04052019f * fVec83[(IOTA0 - 544) & 1023] + 0.0391342f * fVec83[(IOTA0 - 543) & 1023] + 0.03812155f * fVec83[(IOTA0 - 542) & 1023] + 0.03756579f * fVec83[(IOTA0 - 541) & 1023] + 0.03754095f * fVec83[(IOTA0 - 540) & 1023] + 0.03795706f * fVec83[(IOTA0 - 539) & 1023] + 0.03864567f * fVec83[(IOTA0 - 538) & 1023] + 0.03945354f * fVec83[(IOTA0 - 537) & 1023] + 0.04018194f * fVec83[(IOTA0 - 536) & 1023] + 0.04053304f * fVec83[(IOTA0 - 535) & 1023] + 0.04015772f * fVec83[(IOTA0 - 534) & 1023] + 0.03887825f * fVec83[(IOTA0 - 533) & 1023] + 0.03677523f * fVec83[(IOTA0 - 532) & 1023] + 0.03408246f * fVec83[(IOTA0 - 531) & 1023] + 0.03097f * fVec83[(IOTA0 - 530) & 1023] + 0.02756576f * fVec83[(IOTA0 - 529) & 1023] + 0.02406425f * fVec83[(IOTA0 - 528) & 1023] + 0.0207307f * fVec83[(IOTA0 - 527) & 1023] + 0.0178287f * fVec83[(IOTA0 - 526) & 1023] + 0.01561062f * fVec83[(IOTA0 - 525) & 1023] + 0.01434611f * fVec83[(IOTA0 - 524) & 1023] + 0.0142229f * fVec83[(IOTA0 - 523) & 1023] + 0.01521898f * fVec83[(IOTA0 - 522) & 1023] + 0.01711921f * fVec83[(IOTA0 - 521) & 1023] + 0.01969544f * fVec83[(IOTA0 - 520) & 1023] + 0.02274549f * fVec83[(IOTA0 - 519) & 1023] + 0.02597424f * fVec83[(IOTA0 - 518) & 1023] + 0.02900983f * fVec83[(IOTA0 - 517) & 1023] + 0.03154392f * fVec83[(IOTA0 - 516) & 1023] + 0.03341511f * fVec83[(IOTA0 - 515) & 1023] + 0.03455492f * fVec83[(IOTA0 - 514) & 1023] + 0.03493556f * fVec83[(IOTA0 - 513) & 1023] + 0.03464155f * fVec83[(IOTA0 - 512) & 1023] + 0.03386643f * fVec83[(IOTA0 - 511) & 1023] + 0.03280028f * fVec83[(IOTA0 - 510) & 1023] + 0.03158186f * fVec83[(IOTA0 - 509) & 1023] + 0.0303763f * fVec83[(IOTA0 - 508) & 1023] + 0.02940295f * fVec83[(IOTA0 - 507) & 1023] + 0.0288142f * fVec83[(IOTA0 - 506) & 1023] + 0.02856604f * fVec83[(IOTA0 - 505) & 1023] + 0.02857717f * fVec83[(IOTA0 - 504) & 1023] + 0.02885733f * fVec83[(IOTA0 - 503) & 1023] + 0.02942186f * fVec83[(IOTA0 - 502) & 1023] + 0.03017869f * fVec83[(IOTA0 - 501) & 1023] + 0.03099224f * fVec83[(IOTA0 - 500) & 1023] + 0.03181642f * fVec83[(IOTA0 - 499) & 1023] + 0.03267892f * fVec83[(IOTA0 - 498) & 1023] + 0.03360555f * fVec83[(IOTA0 - 497) & 1023] + 0.0346219f * fVec83[(IOTA0 - 496) & 1023] + 0.03587195f * fVec83[(IOTA0 - 495) & 1023] + 0.03749041f * fVec83[(IOTA0 - 494) & 1023] + 0.03946478f * fVec83[(IOTA0 - 493) & 1023] + 0.04166445f * fVec83[(IOTA0 - 492) & 1023] + 0.04397077f * fVec83[(IOTA0 - 491) & 1023] + 0.04627945f * fVec83[(IOTA0 - 490) & 1023] + 0.04836443f * fVec83[(IOTA0 - 489) & 1023] + 0.04991084f * fVec83[(IOTA0 - 488) & 1023] + 0.05076294f * fVec83[(IOTA0 - 487) & 1023] + 0.05095598f * fVec83[(IOTA0 - 486) & 1023] + 0.05057201f * fVec83[(IOTA0 - 485) & 1023] + 0.04962758f * fVec83[(IOTA0 - 484) & 1023] + 0.0482139f * fVec83[(IOTA0 - 483) & 1023] + 0.04649584f * fVec83[(IOTA0 - 482) & 1023] + 0.04457423f * fVec83[(IOTA0 - 481) & 1023] + 0.04241621f * fVec83[(IOTA0 - 480) & 1023] + 0.04001745f * fVec83[(IOTA0 - 479) & 1023] + 0.037526f * fVec83[(IOTA0 - 478) & 1023] + 0.03515171f * fVec83[(IOTA0 - 477) & 1023] + 0.03295352f * fVec83[(IOTA0 - 476) & 1023] + 0.03085432f * fVec83[(IOTA0 - 475) & 1023] + 0.02890714f * fVec83[(IOTA0 - 474) & 1023] + 0.02726742f * fVec83[(IOTA0 - 473) & 1023] + 0.02600081f * fVec83[(IOTA0 - 472) & 1023] + 0.02505503f * fVec83[(IOTA0 - 471) & 1023] + 0.02449556f * fVec83[(IOTA0 - 470) & 1023] + 0.02449865f * fVec83[(IOTA0 - 469) & 1023] + 0.02514524f * fVec83[(IOTA0 - 468) & 1023] + 0.0263251f * fVec83[(IOTA0 - 467) & 1023] + 0.02797346f * fVec83[(IOTA0 - 466) & 1023] + 0.03015904f * fVec83[(IOTA0 - 465) & 1023] + 0.03290718f * fVec83[(IOTA0 - 464) & 1023] + 0.03607401f * fVec83[(IOTA0 - 463) & 1023] + 0.03948616f * fVec83[(IOTA0 - 462) & 1023] + 0.04308381f * fVec83[(IOTA0 - 461) & 1023] + 0.04677749f * fVec83[(IOTA0 - 460) & 1023] + 0.05027528f * fVec83[(IOTA0 - 459) & 1023] + 0.0532932f * fVec83[(IOTA0 - 458) & 1023] + 0.05578416f * fVec83[(IOTA0 - 457) & 1023] + 0.05781847f * fVec83[(IOTA0 - 456) & 1023] + 0.0592581f * fVec83[(IOTA0 - 455) & 1023] + 0.05983165f * fVec83[(IOTA0 - 454) & 1023] + 0.05945645f * fVec83[(IOTA0 - 453) & 1023] + 0.05833234f * fVec83[(IOTA0 - 452) & 1023] + 0.05666568f * fVec83[(IOTA0 - 451) & 1023] + 0.05456352f * fVec83[(IOTA0 - 450) & 1023] + 0.05213287f * fVec83[(IOTA0 - 449) & 1023] + 0.04958099f * fVec83[(IOTA0 - 448) & 1023] + 0.04699549f * fVec83[(IOTA0 - 447) & 1023] + 0.04429333f * fVec83[(IOTA0 - 446) & 1023] + 0.04150293f * fVec83[(IOTA0 - 445) & 1023] + 0.03891545f * fVec83[(IOTA0 - 444) & 1023] + 0.03676942f * fVec83[(IOTA0 - 443) & 1023] + 0.03506742f * fVec83[(IOTA0 - 442) & 1023] + 0.03378512f * fVec83[(IOTA0 - 441) & 1023] + 0.03309922f * fVec83[(IOTA0 - 440) & 1023] + 0.03316955f * fVec83[(IOTA0 - 439) & 1023] + 0.03386087f * fVec83[(IOTA0 - 438) & 1023] + 0.03490454f * fVec83[(IOTA0 - 437) & 1023] + 0.03623937f * fVec83[(IOTA0 - 436) & 1023] + 0.03794804f * fVec83[(IOTA0 - 435) & 1023] + 0.03995022f * fVec83[(IOTA0 - 434) & 1023] + 0.04200678f * fVec83[(IOTA0 - 433) & 1023] + 0.04391491f * fVec83[(IOTA0 - 432) & 1023] + 0.04545069f * fVec83[(IOTA0 - 431) & 1023] + 0.04620801f * fVec83[(IOTA0 - 430) & 1023] + 0.04586643f * fVec83[(IOTA0 - 429) & 1023] + 0.04452419f * fVec83[(IOTA0 - 428) & 1023] + 0.04259603f * fVec83[(IOTA0 - 427) & 1023] + 0.04044295f * fVec83[(IOTA0 - 426) & 1023] + 0.03823067f * fVec83[(IOTA0 - 425) & 1023] + 0.03614347f * fVec83[(IOTA0 - 424) & 1023] + 0.03450511f * fVec83[(IOTA0 - 423) & 1023] + 0.03351385f * fVec83[(IOTA0 - 422) & 1023] + 0.03304892f * fVec83[(IOTA0 - 421) & 1023] + 0.03292472f * fVec83[(IOTA0 - 420) & 1023] + 0.03320204f * fVec83[(IOTA0 - 419) & 1023] + 0.03399694f * fVec83[(IOTA0 - 418) & 1023] + 0.0351978f * fVec83[(IOTA0 - 417) & 1023] + 0.0366273f * fVec83[(IOTA0 - 416) & 1023] + 0.03829617f * fVec83[(IOTA0 - 415) & 1023] + 0.04023051f * fVec83[(IOTA0 - 414) & 1023] + 0.04215335f * fVec83[(IOTA0 - 413) & 1023] + 0.04363895f * fVec83[(IOTA0 - 412) & 1023] + 0.04461675f * fVec83[(IOTA0 - 411) & 1023] + 0.04541658f * fVec83[(IOTA0 - 410) & 1023] + 0.04620764f * fVec83[(IOTA0 - 409) & 1023] + 0.04688204f * fVec83[(IOTA0 - 408) & 1023] + 0.0474965f * fVec83[(IOTA0 - 407) & 1023] + 0.04842536f * fVec83[(IOTA0 - 406) & 1023] + 0.04989551f * fVec83[(IOTA0 - 405) & 1023] + 0.05171899f * fVec83[(IOTA0 - 404) & 1023] + 0.0536296f * fVec83[(IOTA0 - 403) & 1023] + 0.05565897f * fVec83[(IOTA0 - 402) & 1023] + 0.05786173f * fVec83[(IOTA0 - 401) & 1023] + 0.0599021f * fVec83[(IOTA0 - 400) & 1023] + 0.06131293f * fVec83[(IOTA0 - 399) & 1023] + 0.06198275f * fVec83[(IOTA0 - 398) & 1023] + 0.06192727f * fVec83[(IOTA0 - 397) & 1023] + 0.06080216f * fVec83[(IOTA0 - 396) & 1023] + 0.0581891f * fVec83[(IOTA0 - 395) & 1023] + 0.0542254f * fVec83[(IOTA0 - 394) & 1023] + 0.04943269f * fVec83[(IOTA0 - 393) & 1023] + 0.0440516f * fVec83[(IOTA0 - 392) & 1023] + 0.03806705f * fVec83[(IOTA0 - 391) & 1023] + 0.03175945f * fVec83[(IOTA0 - 390) & 1023] + 0.02577676f * fVec83[(IOTA0 - 389) & 1023] + 0.02047983f * fVec83[(IOTA0 - 388) & 1023] + 0.01573544f * fVec83[(IOTA0 - 387) & 1023] + 0.01152865f * fVec83[(IOTA0 - 386) & 1023] + 0.00819818f * fVec83[(IOTA0 - 385) & 1023] + 0.00584564f * fVec83[(IOTA0 - 384) & 1023] + 0.00409186f * fVec83[(IOTA0 - 383) & 1023] + 0.00267992f * fVec83[(IOTA0 - 382) & 1023] + 0.0018152f * fVec83[(IOTA0 - 381) & 1023] + 0.00165318f * fVec83[(IOTA0 - 380) & 1023] + 0.00185524f * fVec83[(IOTA0 - 379) & 1023] + 0.00197524f * fVec83[(IOTA0 - 378) & 1023] + 0.00195992f * fVec83[(IOTA0 - 377) & 1023] + 0.00197166f * fVec83[(IOTA0 - 376) & 1023] + 0.00188725f * fVec83[(IOTA0 - 375) & 1023] + 0.00145285f * fVec83[(IOTA0 - 374) & 1023] + 0.00076536f * fVec83[(IOTA0 - 373) & 1023] + 0.00015188f * fVec83[(IOTA0 - 372) & 1023] + 0.00014744f * fVec83[(IOTA0 - 362) & 1023] + 1.36e-06f * fVec83[(IOTA0 - 361) & 1023] + 0.0012739f * fVec83[(IOTA0 - 330) & 1023] + 0.0088229f * fVec83[(IOTA0 - 329) & 1023] + 0.01628354f * fVec83[(IOTA0 - 328) & 1023] + 0.02286796f * fVec83[(IOTA0 - 327) & 1023] + 0.02829923f * fVec83[(IOTA0 - 326) & 1023] + 0.03223611f * fVec83[(IOTA0 - 325) & 1023] + 0.03397037f * fVec83[(IOTA0 - 324) & 1023] + 0.03322207f * fVec83[(IOTA0 - 323) & 1023] + 0.03058899f * fVec83[(IOTA0 - 322) & 1023] + 0.0267129f * fVec83[(IOTA0 - 321) & 1023] + 0.02170108f * fVec83[(IOTA0 - 320) & 1023] + 0.01580761f * fVec83[(IOTA0 - 319) & 1023] + 0.0099291f * fVec83[(IOTA0 - 318) & 1023] + 0.00495979f * fVec83[(IOTA0 - 317) & 1023] + 0.00087979f * fVec83[(IOTA0 - 316) & 1023] + 0.0016119f * fVec83[(IOTA0 - 303) & 1023] + 0.00458348f * fVec83[(IOTA0 - 302) & 1023] + 0.00697297f * fVec83[(IOTA0 - 301) & 1023] + 0.00888111f * fVec83[(IOTA0 - 300) & 1023] + 0.00987967f * fVec83[(IOTA0 - 299) & 1023] + 0.00952683f * fVec83[(IOTA0 - 298) & 1023] + 0.00814764f * fVec83[(IOTA0 - 297) & 1023] + 0.00646974f * fVec83[(IOTA0 - 296) & 1023] + 0.00467431f * fVec83[(IOTA0 - 295) & 1023] + 0.00247514f * fVec83[(IOTA0 - 294) & 1023] + 0.00025112f * fVec83[(IOTA0 - 293) & 1023] + 0.00120297f * fVec83[(IOTA0 - 287) & 1023] + 0.00212119f * fVec83[(IOTA0 - 286) & 1023] + 0.00252358f * fVec83[(IOTA0 - 285) & 1023] + 0.00252099f * fVec83[(IOTA0 - 284) & 1023] + 0.00206484f * fVec83[(IOTA0 - 283) & 1023] + 0.00089734f * fVec83[(IOTA0 - 282) & 1023] + 3.226e-05f * fVec83[(IOTA0 - 276) & 1023] + 0.00062002f * fVec83[(IOTA0 - 275) & 1023] + 0.00054043f * fVec83[(IOTA0 - 274) & 1023] + 0.00638064f * fVec83[(IOTA0 - 172) & 1023] + 0.01034854f * fVec83[(IOTA0 - 171) & 1023] + 0.0104809f * fVec83[(IOTA0 - 170) & 1023] + 0.005629f * fVec83[(IOTA0 - 169) & 1023] + 0.00976819f * fVec83[(IOTA0 - 150) & 1023] + 0.03955809f * fVec83[(IOTA0 - 149) & 1023] + 0.06796286f * fVec83[(IOTA0 - 148) & 1023] + 0.09001875f * fVec83[(IOTA0 - 147) & 1023] + 0.10318525f * fVec83[(IOTA0 - 146) & 1023] + 0.10701907f * fVec83[(IOTA0 - 145) & 1023] + 0.09930608f * fVec83[(IOTA0 - 144) & 1023] + 0.07763861f * fVec83[(IOTA0 - 143) & 1023] + 0.04433436f * fVec83[(IOTA0 - 142) & 1023] + 0.00520758f * fVec83[(IOTA0 - 141) & 1023] + 0.01397412f * fVec83[(IOTA0 - 118) & 1023] + 0.03231657f * fVec83[(IOTA0 - 117) & 1023] + 0.04471005f * fVec83[(IOTA0 - 116) & 1023] + 0.05164051f * fVec83[(IOTA0 - 115) & 1023] + 0.04969135f * fVec83[(IOTA0 - 114) & 1023] + 0.03605856f * fVec83[(IOTA0 - 113) & 1023] + 0.01217301f * fVec83[(IOTA0 - 112) & 1023] + 0.00543967f * fVec83[(IOTA0 - 98) & 1023] + 0.02673947f * fVec83[(IOTA0 - 97) & 1023] + 0.03648975f * fVec83[(IOTA0 - 96) & 1023] + 0.03577828f * fVec83[(IOTA0 - 95) & 1023] + 0.0285894f * fVec83[(IOTA0 - 94) & 1023] + 0.01811764f * fVec83[(IOTA0 - 93) & 1023] + 0.00532795f * fVec83[(IOTA0 - 92) & 1023] + 0.00157063f * fVec83[(IOTA0 - 68) & 1023] + 0.01429853f * fVec83[(IOTA0 - 67) & 1023] + 0.01624448f * fVec83[(IOTA0 - 66) & 1023] + 0.01332024f * fVec83[(IOTA0 - 65) & 1023] + 0.01038475f * fVec83[(IOTA0 - 64) & 1023] + 0.0102325f * fVec83[(IOTA0 - 63) & 1023] + 0.01570949f * fVec83[(IOTA0 - 62) & 1023] + 0.02621325f * fVec83[(IOTA0 - 61) & 1023] + 0.03449399f * fVec83[(IOTA0 - 60) & 1023] + 0.03316893f * fVec83[(IOTA0 - 59) & 1023] + 0.02141486f * fVec83[(IOTA0 - 58) & 1023] + 0.00168124f * fVec83[(IOTA0 - 57) & 1023] + 0.01952242f * fVec83[(IOTA0 - 46) & 1023] + 0.06704315f * fVec83[(IOTA0 - 45) & 1023] + 0.10955748f * fVec83[(IOTA0 - 44) & 1023] + 0.14042453f * fVec83[(IOTA0 - 43) & 1023] + 0.1544885f * fVec83[(IOTA0 - 42) & 1023] + 0.15123281f * fVec83[(IOTA0 - 41) & 1023] + 0.1324053f * fVec83[(IOTA0 - 40) & 1023] + 0.09952952f * fVec83[(IOTA0 - 39) & 1023] + 0.05870358f * fVec83[(IOTA0 - 38) & 1023] + 0.02383884f * fVec83[(IOTA0 - 37) & 1023] + 0.00675707f * fVec83[(IOTA0 - 36) & 1023] + 0.00848626f * fVec83[(IOTA0 - 35) & 1023] + 0.02783863f * fVec83[(IOTA0 - 34) & 1023] + 0.06788524f * fVec83[(IOTA0 - 33) & 1023] + 0.12388333f * fVec83[(IOTA0 - 32) & 1023] + 0.1746286f * fVec83[(IOTA0 - 31) & 1023] + 0.19615036f * fVec83[(IOTA0 - 30) & 1023] + 0.1782187f * fVec83[(IOTA0 - 29) & 1023] + 0.12192415f * fVec83[(IOTA0 - 28) & 1023] + 0.02822396f * fVec83[(IOTA0 - 27) & 1023] + 0.05870185f * fVec83[(IOTA0 - 16) & 1023] + 0.3216985f * fVec83[(IOTA0 - 15) & 1023] + 0.56135064f * fVec83[(IOTA0 - 14) & 1023] + 0.74857503f * fVec83[(IOTA0 - 13) & 1023] + 0.8785815f * fVec83[(IOTA0 - 12) & 1023] + 0.96131873f * fVec83[(IOTA0 - 11) & 1023] + fVec83[(IOTA0 - 10) & 1023] + 0.9850414f * fVec83[(IOTA0 - 9) & 1023] + 0.90463525f * fVec83[(IOTA0 - 8) & 1023] + 0.76035035f * fVec83[(IOTA0 - 7) & 1023] + 0.5756886f * fVec83[(IOTA0 - 6) & 1023] + 0.38834506f * fVec83[(IOTA0 - 5) & 1023] + 0.2322759f * fVec83[(IOTA0 - 4) & 1023] + 0.12389284f * fVec83[(IOTA0 - 3) & 1023] + 0.05985266f * fVec83[(IOTA0 - 2) & 1023] + 0.02638726f * fVec83[(IOTA0 - 1) & 1023] + 0.0116741f * fVec82[(IOTA0 - 639) & 1023] + 0.00923925f * fVec82[(IOTA0 - 638) & 1023] + 0.00714288f * fVec82[(IOTA0 - 637) & 1023] + 0.0058497f * fVec82[(IOTA0 - 636) & 1023] + 0.00393625f * fVec82[(IOTA0 - 635) & 1023] + 0.00259896f * fVec82[(IOTA0 - 634) & 1023] + 0.00320871f * fVec82[(IOTA0 - 633) & 1023] + 0.00400138f * fVec82[(IOTA0 - 632) & 1023] + 0.00434206f * fVec82[(IOTA0 - 631) & 1023] + 0.00546491f * fVec82[(IOTA0 - 630) & 1023] + 0.00679793f * fVec82[(IOTA0 - 629) & 1023] + 0.00667598f * fVec82[(IOTA0 - 628) & 1023] + 0.00550059f * fVec82[(IOTA0 - 627) & 1023] + 0.00471842f * fVec82[(IOTA0 - 626) & 1023] + 0.00350723f * fVec82[(IOTA0 - 625) & 1023] + 0.00099454f * fVec82[(IOTA0 - 624) & 1023] + 0.00015983f * fVec82[(IOTA0 - 618) & 1023] + 0.00090204f * fVec82[(IOTA0 - 616) & 1023] + 0.00135624f * fVec82[(IOTA0 - 615) & 1023] + 0.00081162f * fVec82[(IOTA0 - 614) & 1023] + 0.00072327f * fVec82[(IOTA0 - 613) & 1023] + 0.00115229f * fVec82[(IOTA0 - 612) & 1023] + 0.0022204f * fVec82[(IOTA0 - 611) & 1023] + 0.00448087f * fVec82[(IOTA0 - 610) & 1023] + 0.00739937f * fVec82[(IOTA0 - 609) & 1023] + 0.00964506f * fVec82[(IOTA0 - 608) & 1023] + 0.01126829f * fVec82[(IOTA0 - 607) & 1023] + 0.01178762f * fVec82[(IOTA0 - 606) & 1023] + 0.01000456f * fVec82[(IOTA0 - 605) & 1023] + 0.00754441f * fVec82[(IOTA0 - 604) & 1023] + 0.00448295f * fVec82[(IOTA0 - 603) & 1023] + 0.0005236f * (fVec82[(IOTA0 - 573) & 1023] - fVec82[(IOTA0 - 602) & 1023]) + 0.00136039f * fVec82[(IOTA0 - 584) & 1023] + 0.00231925f * fVec82[(IOTA0 - 583) & 1023] + 0.00177463f * fVec82[(IOTA0 - 582) & 1023] + 0.00249595f * fVec82[(IOTA0 - 572) & 1023] + 0.00369443f * fVec82[(IOTA0 - 571) & 1023] + 0.00429575f * fVec82[(IOTA0 - 570) & 1023] + 0.0040582f * fVec82[(IOTA0 - 569) & 1023] + 0.00320455f * fVec82[(IOTA0 - 568) & 1023] + 0.00226463f * fVec82[(IOTA0 - 567) & 1023] + 0.00147819f * fVec82[(IOTA0 - 566) & 1023] + 0.00046043f * fVec82[(IOTA0 - 565) & 1023] + 0.0068253f * (fVec82[(IOTA0 - 284) & 1023] - fVec82[(IOTA0 - 557) & 1023]) + 0.00205433f * (fVec82[(IOTA0 - 529) & 1023] - fVec82[(IOTA0 - 532) & 1023]) + 0.00048572f * fVec82[(IOTA0 - 530) & 1023] + 0.00366914f * fVec82[(IOTA0 - 528) & 1023] + 0.0040224f * fVec82[(IOTA0 - 527) & 1023] + 0.00247908f * fVec82[(IOTA0 - 526) & 1023] + 0.00147403f * fVec82[(IOTA0 - 525) & 1023] + 0.00209221f * fVec82[(IOTA0 - 524) & 1023] + 0.00289748f * fVec82[(IOTA0 - 523) & 1023] + 0.00388994f * (fVec82[(IOTA0 - 511) & 1023] + fVec82[(IOTA0 - 522) & 1023]) + 0.00532402f * fVec82[(IOTA0 - 521) & 1023] + 0.00681907f * fVec82[(IOTA0 - 520) & 1023] + 0.00858734f * fVec82[(IOTA0 - 519) & 1023] + 0.00994786f * fVec82[(IOTA0 - 518) & 1023] + 0.01094032f * fVec82[(IOTA0 - 517) & 1023] + 0.01111482f * fVec82[(IOTA0 - 515) & 1023] + 0.01098651f * fVec82[(IOTA0 - 514) & 1023] + 0.00943268f * fVec82[(IOTA0 - 513) & 1023] + 0.00663822f * fVec82[(IOTA0 - 512) & 1023] + 0.0006392f * fVec82[(IOTA0 - 510) & 1023] + 0.00169692f * fVec82[(IOTA0 - 493) & 1023] + 0.00497075f * fVec82[(IOTA0 - 492) & 1023] + 0.00840442f * fVec82[(IOTA0 - 491) & 1023] + 0.01114426f * fVec82[(IOTA0 - 490) & 1023] + 0.01191385f * fVec82[(IOTA0 - 489) & 1023] + 0.01231122f * fVec82[(IOTA0 - 488) & 1023] + 0.01304292f * fVec82[(IOTA0 - 487) & 1023] + 0.01244576f * fVec82[(IOTA0 - 486) & 1023] + 0.01071952f * fVec82[(IOTA0 - 485) & 1023] + 0.00896163f * fVec82[(IOTA0 - 484) & 1023] + 0.00675381f * fVec82[(IOTA0 - 483) & 1023] + 0.00411075f * fVec82[(IOTA0 - 482) & 1023] + 0.00118174f * fVec82[(IOTA0 - 481) & 1023] + 0.00066865f * fVec82[(IOTA0 - 476) & 1023] + 0.00261997f * fVec82[(IOTA0 - 475) & 1023] + 0.00564148f * fVec82[(IOTA0 - 474) & 1023] + 0.00914455f * fVec82[(IOTA0 - 473) & 1023] + 0.01169939f * fVec82[(IOTA0 - 472) & 1023] + 0.01421f * fVec82[(IOTA0 - 471) & 1023] + 0.01681104f * fVec82[(IOTA0 - 470) & 1023] + 0.01824083f * fVec82[(IOTA0 - 469) & 1023] + 0.01802418f * fVec82[(IOTA0 - 468) & 1023] + 0.01649981f * fVec82[(IOTA0 - 467) & 1023] + 0.01382301f * fVec82[(IOTA0 - 466) & 1023] + 0.00991841f * fVec82[(IOTA0 - 465) & 1023] + 0.00541444f * fVec82[(IOTA0 - 464) & 1023] + 0.00050466f * fVec82[(IOTA0 - 451) & 1023] + 0.00092098f * fVec82[(IOTA0 - 450) & 1023] + 0.00084314f * fVec82[(IOTA0 - 449) & 1023] + 0.00118382f * fVec82[(IOTA0 - 448) & 1023] + 0.00074221f * fVec82[(IOTA0 - 447) & 1023] + 0.00074013f * fVec82[(IOTA0 - 427) & 1023] + 0.00351774f * fVec82[(IOTA0 - 426) & 1023] + 0.00573398f * fVec82[(IOTA0 - 425) & 1023] + 0.00726691f * fVec82[(IOTA0 - 424) & 1023] + 0.00751289f * fVec82[(IOTA0 - 423) & 1023] + 0.00717013f * fVec82[(IOTA0 - 422) & 1023] + 0.00596957f * fVec82[(IOTA0 - 421) & 1023] + 0.00382689f * fVec82[(IOTA0 - 420) & 1023] + 0.00229823f * fVec82[(IOTA0 - 419) & 1023] + 0.00127217f * fVec82[(IOTA0 - 418) & 1023] + 0.00204383f * fVec82[(IOTA0 - 414) & 1023] + 0.00418858f * fVec82[(IOTA0 - 413) & 1023] + 0.00507376f * fVec82[(IOTA0 - 412) & 1023] + 0.00460062f * fVec82[(IOTA0 - 411) & 1023] + 0.00148454f * fVec82[(IOTA0 - 410) & 1023] + 0.00074856f * fVec82[(IOTA0 - 395) & 1023] + 0.00054878f * fVec82[(IOTA0 - 394) & 1023] + 9.886e-05f * fVec82[(IOTA0 - 393) & 1023] + 0.00018293f * fVec82[(IOTA0 - 392) & 1023] + 0.0011859f * fVec82[(IOTA0 - 391) & 1023] + 0.00225828f * fVec82[(IOTA0 - 390) & 1023] + 0.0015245f * fVec82[(IOTA0 - 389) & 1023] + 0.00023131f * fVec82[(IOTA0 - 388) & 1023] + 0.00443248f * fVec82[(IOTA0 - 368) & 1023] + 0.0093023f * fVec82[(IOTA0 - 367) & 1023] + 0.01439073f * fVec82[(IOTA0 - 366) & 1023] + 0.01854143f * fVec82[(IOTA0 - 365) & 1023] + 0.02030983f * fVec82[(IOTA0 - 364) & 1023] + 0.02120772f * fVec82[(IOTA0 - 363) & 1023] + 0.02108369f * fVec82[(IOTA0 - 362) & 1023] + 0.0193594f * fVec82[(IOTA0 - 361) & 1023] + 0.01717689f * fVec82[(IOTA0 - 360) & 1023] + 0.01535802f * fVec82[(IOTA0 - 359) & 1023] + 0.01301347f * (fVec82[(IOTA0 - 232) & 1023] + fVec82[(IOTA0 - 358) & 1023]) + 0.01001934f * fVec82[(IOTA0 - 357) & 1023] + 0.00762641f * fVec82[(IOTA0 - 356) & 1023] + 0.00377007f * fVec82[(IOTA0 - 355) & 1023] + 0.00316667f * fVec82[(IOTA0 - 347) & 1023] + 0.00795242f * fVec82[(IOTA0 - 346) & 1023] + 0.01285584f * fVec82[(IOTA0 - 345) & 1023] + 0.01685515f * fVec82[(IOTA0 - 344) & 1023] + 0.01808308f * fVec82[(IOTA0 - 343) & 1023] + 0.01730507f * fVec82[(IOTA0 - 342) & 1023] + 0.0163968f * fVec82[(IOTA0 - 341) & 1023] + 0.01337945f * fVec82[(IOTA0 - 340) & 1023] + 0.00856009f * fVec82[(IOTA0 - 339) & 1023] + 0.0055111f * fVec82[(IOTA0 - 338) & 1023] + 0.003686f * fVec82[(IOTA0 - 337) & 1023] + 0.00123013f * fVec82[(IOTA0 - 336) & 1023] + 0.00074649f * fVec82[(IOTA0 - 316) & 1023] + 0.00658348f * fVec82[(IOTA0 - 315) & 1023] + 0.01109587f * fVec82[(IOTA0 - 314) & 1023] + 0.01513098f * fVec82[(IOTA0 - 313) & 1023] + 0.0171621f * fVec82[(IOTA0 - 312) & 1023] + 0.0173493f * fVec82[(IOTA0 - 311) & 1023] + 0.01692028f * fVec82[(IOTA0 - 310) & 1023] + 0.01449386f * fVec82[(IOTA0 - 309) & 1023] + 0.01078465f * fVec82[(IOTA0 - 308) & 1023] + 0.00618818f * fVec82[(IOTA0 - 307) & 1023] + 0.0008242f * fVec82[(IOTA0 - 306) & 1023] + 4.839e-05f * fVec82[(IOTA0 - 303) & 1023] + 0.00336646f * fVec82[(IOTA0 - 302) & 1023] + 0.00891544f * fVec82[(IOTA0 - 301) & 1023] + 0.01209469f * fVec82[(IOTA0 - 300) & 1023] + 0.01504471f * fVec82[(IOTA0 - 299) & 1023] + 0.01755948f * fVec82[(IOTA0 - 298) & 1023] + 0.01611502f * fVec82[(IOTA0 - 297) & 1023] + 0.01409013f * fVec82[(IOTA0 - 296) & 1023] + 0.01273389f * fVec82[(IOTA0 - 295) & 1023] + 0.00901417f * fVec82[(IOTA0 - 294) & 1023] + 0.00516419f * fVec82[(IOTA0 - 293) & 1023] + 0.00459647f * fVec82[(IOTA0 - 292) & 1023] + 0.00410439f * fVec82[(IOTA0 - 291) & 1023] + 0.00196183f * fVec82[(IOTA0 - 290) & 1023] + 0.0019176f * fVec82[(IOTA0 - 289) & 1023] + 0.00348621f * fVec82[(IOTA0 - 288) & 1023] + 0.00350943f * fVec82[(IOTA0 - 287) & 1023] + 0.00436308f * fVec82[(IOTA0 - 286) & 1023] + 0.00678742f * fVec82[(IOTA0 - 285) & 1023] + 0.00634385f * fVec82[(IOTA0 - 283) & 1023] + 0.00616093f * fVec82[(IOTA0 - 282) & 1023] + 0.00383947f * fVec82[(IOTA0 - 281) & 1023] + 0.00178099f * fVec82[(IOTA0 - 280) & 1023] + 0.0039657f * fVec82[(IOTA0 - 272) & 1023] + 0.00879349f * fVec82[(IOTA0 - 271) & 1023] + 0.01357703f * fVec82[(IOTA0 - 270) & 1023] + 0.01618858f * fVec82[(IOTA0 - 269) & 1023] + 0.01629159f * fVec82[(IOTA0 - 268) & 1023] + 0.01500268f * fVec82[(IOTA0 - 267) & 1023] + 0.01329318f * fVec82[(IOTA0 - 266) & 1023] + 0.01095706f * fVec82[(IOTA0 - 265) & 1023] + 0.00773369f * fVec82[(IOTA0 - 264) & 1023] + 0.00381003f * fVec82[(IOTA0 - 263) & 1023] + 0.00011144f * fVec82[(IOTA0 - 257) & 1023] + 0.00838768f * fVec82[(IOTA0 - 256) & 1023] + 0.01962432f * fVec82[(IOTA0 - 255) & 1023] + 0.02971307f * fVec82[(IOTA0 - 254) & 1023] + 0.03835515f * fVec82[(IOTA0 - 253) & 1023] + 0.04526244f * fVec82[(IOTA0 - 252) & 1023] + 0.04766161f * fVec82[(IOTA0 - 251) & 1023] + 0.0464525f * fVec82[(IOTA0 - 250) & 1023] + 0.04229972f * fVec82[(IOTA0 - 249) & 1023] + 0.03302906f * fVec82[(IOTA0 - 248) & 1023] + 0.02136119f * fVec82[(IOTA0 - 247) & 1023] + 0.01076571f * fVec82[(IOTA0 - 246) & 1023] + 0.00035962f * fVec82[(IOTA0 - 245) & 1023] + 0.00577614f * fVec82[(IOTA0 - 1) & 1023] + 0.01422051f * fVec82[(IOTA0 - 2) & 1023] + 0.02337142f * fVec82[(IOTA0 - 3) & 1023] + 0.03243813f * fVec82[(IOTA0 - 4) & 1023] + 0.04075005f * fVec82[(IOTA0 - 5) & 1023] + 0.04903679f * fVec82[(IOTA0 - 6) & 1023] + 0.05928317f * fVec82[(IOTA0 - 7) & 1023] + 0.07050735f * fVec82[(IOTA0 - 8) & 1023] + 0.08201526f * fVec82[(IOTA0 - 9) & 1023] + 0.11265139f * fVec82[(IOTA0 - 10) & 1023] + 0.19815506f * fVec82[(IOTA0 - 11) & 1023] + 0.35875827f * fVec82[(IOTA0 - 12) & 1023] + 0.58793646f * fVec82[(IOTA0 - 13) & 1023] + 0.8291167f * fVec82[(IOTA0 - 14) & 1023] + 0.9780017f * fVec82[(IOTA0 - 15) & 1023] + fVec82[(IOTA0 - 16) & 1023] + 0.9167062f * fVec82[(IOTA0 - 17) & 1023] + 0.7079977f * fVec82[(IOTA0 - 18) & 1023] + 0.39776522f * fVec82[(IOTA0 - 19) & 1023] + 0.04442346f * fVec82[(IOTA0 - 20) & 1023] + 0.0246288f * fVec82[(IOTA0 - 36) & 1023] + 0.05391504f * fVec82[(IOTA0 - 37) & 1023] + 0.05492217f * fVec82[(IOTA0 - 38) & 1023] + 0.02351854f * fVec82[(IOTA0 - 39) & 1023] + 0.01737875f * fVec82[(IOTA0 - 59) & 1023] + 0.0139303f * fVec82[(IOTA0 - 60) & 1023] + 0.01908605f * fVec82[(IOTA0 - 78) & 1023] + 0.05028574f * fVec82[(IOTA0 - 79) & 1023] + 0.07539183f * fVec82[(IOTA0 - 80) & 1023] + 0.08473201f * fVec82[(IOTA0 - 81) & 1023] + 0.10673241f * fVec82[(IOTA0 - 82) & 1023] + 0.10990739f * fVec82[(IOTA0 - 83) & 1023] + 0.07290652f * fVec82[(IOTA0 - 84) & 1023] + 0.04873192f * fVec82[(IOTA0 - 85) & 1023] + 0.03219215f * fVec82[(IOTA0 - 86) & 1023] + 0.00579495f * fVec82[(IOTA0 - 87) & 1023] + 0.01899355f * fVec82[(IOTA0 - 97) & 1023] + 0.05675777f * fVec82[(IOTA0 - 98) & 1023] + 0.08011232f * fVec82[(IOTA0 - 99) & 1023] + 0.07456762f * fVec82[(IOTA0 - 100) & 1023] + 0.06912166f * fVec82[(IOTA0 - 101) & 1023] + 0.05880368f * fVec82[(IOTA0 - 102) & 1023] + 0.03589709f * fVec82[(IOTA0 - 103) & 1023] + 0.0182555f * fVec82[(IOTA0 - 104) & 1023] + 0.00146132f * fVec82[(IOTA0 - 105) & 1023] + 0.00609152f * fVec82[(IOTA0 - 113) & 1023] + 0.0112788f * fVec82[(IOTA0 - 114) & 1023] + 0.01476501f * fVec82[(IOTA0 - 115) & 1023] + 0.02420613f * fVec82[(IOTA0 - 116) & 1023] + 0.03900278f * fVec82[(IOTA0 - 117) & 1023] + 0.05086604f * fVec82[(IOTA0 - 118) & 1023] + 0.06693059f * fVec82[(IOTA0 - 119) & 1023] + 0.0866622f * fVec82[(IOTA0 - 120) & 1023] + 0.09795371f * fVec82[(IOTA0 - 121) & 1023] + 0.10768063f * fVec82[(IOTA0 - 122) & 1023] + 0.11223091f * fVec82[(IOTA0 - 123) & 1023] + 0.1031873f * fVec82[(IOTA0 - 124) & 1023] + 0.09207456f * fVec82[(IOTA0 - 125) & 1023] + 0.08129199f * fVec82[(IOTA0 - 126) & 1023] + 0.0623384f * fVec82[(IOTA0 - 127) & 1023] + 0.04166895f * fVec82[(IOTA0 - 128) & 1023] + 0.02679237f * fVec82[(IOTA0 - 129) & 1023] + 0.01387348f * fVec82[(IOTA0 - 130) & 1023] + 0.00674331f * fVec82[(IOTA0 - 131) & 1023] + 0.00553431f * fVec82[(IOTA0 - 132) & 1023] + 0.00125323f * fVec82[(IOTA0 - 141) & 1023] + 0.00371337f * fVec82[(IOTA0 - 142) & 1023] + 0.00442198f * fVec82[(IOTA0 - 143) & 1023] + 0.00910044f * fVec82[(IOTA0 - 144) & 1023] + 0.00763068f * fVec82[(IOTA0 - 145) & 1023] + 0.00272714f * fVec82[(IOTA0 - 146) & 1023] + 0.00536605f * fVec82[(IOTA0 - 147) & 1023] + 0.0092665f * fVec82[(IOTA0 - 148) & 1023] + 0.0142331f * fVec82[(IOTA0 - 149) & 1023] + 0.02510194f * fVec82[(IOTA0 - 150) & 1023] + 0.03244241f * fVec82[(IOTA0 - 151) & 1023] + 0.03527891f * fVec82[(IOTA0 - 152) & 1023] + 0.04072695f * fVec82[(IOTA0 - 153) & 1023] + 0.04508368f * fVec82[(IOTA0 - 154) & 1023] + 0.04325442f * fVec82[(IOTA0 - 155) & 1023] + 0.04168789f * fVec82[(IOTA0 - 156) & 1023] + 0.04266557f * fVec82[(IOTA0 - 157) & 1023] + 0.04183501f * fVec82[(IOTA0 - 158) & 1023] + 0.04293891f * fVec82[(IOTA0 - 159) & 1023] + 0.04425739f * fVec82[(IOTA0 - 160) & 1023] + 0.04165636f * fVec82[(IOTA0 - 161) & 1023] + 0.03892067f * fVec82[(IOTA0 - 162) & 1023] + 0.03234771f * fVec82[(IOTA0 - 163) & 1023] + 0.01744596f * fVec82[(IOTA0 - 164) & 1023] + 0.00027127f * fVec82[(IOTA0 - 165) & 1023] + 0.01332898f * fVec82[(IOTA0 - 173) & 1023] + 0.03122486f * fVec82[(IOTA0 - 174) & 1023] + 0.04595211f * fVec82[(IOTA0 - 175) & 1023] + 0.05188796f * fVec82[(IOTA0 - 176) & 1023] + 0.05061799f * fVec82[(IOTA0 - 177) & 1023] + 0.04767847f * fVec82[(IOTA0 - 178) & 1023] + 0.04309667f * fVec82[(IOTA0 - 179) & 1023] + 0.04010669f * fVec82[(IOTA0 - 180) & 1023] + 0.03973449f * fVec82[(IOTA0 - 181) & 1023] + 0.03456822f * fVec82[(IOTA0 - 182) & 1023] + 0.02560868f * fVec82[(IOTA0 - 183) & 1023] + 0.01674371f * fVec82[(IOTA0 - 184) & 1023] + 0.00241811f * fVec82[(IOTA0 - 185) & 1023] + 0.02101221f * fVec82[(IOTA0 - 194) & 1023] + 0.03799981f * fVec82[(IOTA0 - 195) & 1023] + 0.05258825f * fVec82[(IOTA0 - 196) & 1023] + 0.05725193f * fVec82[(IOTA0 - 197) & 1023] + 0.05251884f * fVec82[(IOTA0 - 198) & 1023] + 0.0451152f * fVec82[(IOTA0 - 199) & 1023] + 0.0354534f * fVec82[(IOTA0 - 200) & 1023] + 0.02544457f * fVec82[(IOTA0 - 201) & 1023] + 0.02046966f * fVec82[(IOTA0 - 202) & 1023] + 0.01967271f * fVec82[(IOTA0 - 203) & 1023] + 0.01892842f * fVec82[(IOTA0 - 204) & 1023] + 0.0181399f * fVec82[(IOTA0 - 205) & 1023] + 0.01429615f * fVec82[(IOTA0 - 206) & 1023] + 0.0035704f * fVec82[(IOTA0 - 207) & 1023] + 0.0071639f * fVec82[(IOTA0 - 217) & 1023] + 0.01471674f * fVec82[(IOTA0 - 218) & 1023] + 0.01668481f * fVec82[(IOTA0 - 219) & 1023] + 0.01521921f * fVec82[(IOTA0 - 220) & 1023] + 0.011918f * fVec82[(IOTA0 - 221) & 1023] + 0.00656881f * fVec82[(IOTA0 - 222) & 1023] + 0.00314773f * fVec82[(IOTA0 - 223) & 1023] + 0.00379744f * fVec82[(IOTA0 - 224) & 1023] + 0.00602835f * fVec82[(IOTA0 - 225) & 1023] + 0.01027167f * fVec82[(IOTA0 - 226) & 1023] + 0.01706117f * fVec82[(IOTA0 - 227) & 1023] + 0.02130657f * fVec82[(IOTA0 - 228) & 1023] + 0.02322209f * fVec82[(IOTA0 - 229) & 1023] + 0.02393913f * fVec82[(IOTA0 - 230) & 1023] + 0.01963483f * fVec82[(IOTA0 - 231) & 1023] + 0.00699771f * fVec82[(IOTA0 - 233) & 1023] - (0.00975274f * fVec81[(IOTA0 - 639) & 1023] + 0.01224972f * fVec81[(IOTA0 - 638) & 1023] + 0.01436919f * fVec81[(IOTA0 - 637) & 1023] + 0.01517656f * fVec81[(IOTA0 - 636) & 1023] + 0.01500284f * fVec81[(IOTA0 - 635) & 1023] + 0.01448793f * fVec81[(IOTA0 - 634) & 1023] + 0.01362184f * fVec81[(IOTA0 - 633) & 1023] + 0.01278183f * fVec81[(IOTA0 - 632) & 1023] + 0.01335907f * fVec81[(IOTA0 - 631) & 1023] + 0.0142496f * fVec81[(IOTA0 - 630) & 1023] + 0.01439263f * fVec81[(IOTA0 - 629) & 1023] + 0.01425883f * fVec81[(IOTA0 - 628) & 1023] + 0.01337895f * fVec81[(IOTA0 - 627) & 1023] + 0.01139293f * fVec81[(IOTA0 - 626) & 1023] + 0.00934539f * fVec81[(IOTA0 - 625) & 1023] + 0.00722668f * fVec81[(IOTA0 - 624) & 1023] + 0.00532686f * fVec81[(IOTA0 - 623) & 1023] + 0.00467889f * fVec81[(IOTA0 - 622) & 1023] + 0.00448898f * fVec81[(IOTA0 - 621) & 1023] + 0.0045157f * fVec81[(IOTA0 - 620) & 1023] + 0.00435095f * fVec81[(IOTA0 - 619) & 1023] + 0.00362284f * fVec81[(IOTA0 - 618) & 1023] + 0.00218413f * fVec81[(IOTA0 - 617) & 1023] + 0.00086984f * fVec81[(IOTA0 - 616) & 1023] + 9.043e-05f * fVec81[(IOTA0 - 610) & 1023] + 0.0010875f * fVec81[(IOTA0 - 609) & 1023] + 0.00129472f * fVec81[(IOTA0 - 608) & 1023] + 0.00125065f * fVec81[(IOTA0 - 607) & 1023] + 0.00105912f * fVec81[(IOTA0 - 606) & 1023] + 0.00020475f * fVec81[(IOTA0 - 605) & 1023] + 0.00105487f * fVec81[(IOTA0 - 600) & 1023] + 0.00243199f * fVec81[(IOTA0 - 599) & 1023] + 0.00281572f * fVec81[(IOTA0 - 598) & 1023] + 0.00327485f * fVec81[(IOTA0 - 597) & 1023] + 0.00322643f * fVec81[(IOTA0 - 596) & 1023] + 0.00253117f * fVec81[(IOTA0 - 595) & 1023] + 0.00185061f * fVec81[(IOTA0 - 594) & 1023] + 0.00114858f * fVec81[(IOTA0 - 593) & 1023] + 0.0006577f * fVec81[(IOTA0 - 592) & 1023] + 0.00054195f * fVec81[(IOTA0 - 591) & 1023] + 0.00101012f * fVec81[(IOTA0 - 590) & 1023] + 0.00104345f * fVec81[(IOTA0 - 589) & 1023] + 0.00031332f * fVec81[(IOTA0 - 588) & 1023] + 0.00033706f * fVec81[(IOTA0 - 587) & 1023] + 0.00078794f * fVec81[(IOTA0 - 586) & 1023] + 0.00136982f * fVec81[(IOTA0 - 585) & 1023] + 0.00262659f * fVec81[(IOTA0 - 584) & 1023] + 0.00439275f * fVec81[(IOTA0 - 583) & 1023] + 0.00451602f * fVec81[(IOTA0 - 582) & 1023] + 0.00266058f * fVec81[(IOTA0 - 581) & 1023] + 0.00458464f * fVec81[(IOTA0 - 572) & 1023] + 0.00898699f * fVec81[(IOTA0 - 571) & 1023] + 0.01211386f * fVec81[(IOTA0 - 570) & 1023] + 0.01372115f * fVec81[(IOTA0 - 569) & 1023] + 0.01482947f * fVec81[(IOTA0 - 568) & 1023] + 0.01643534f * fVec81[(IOTA0 - 567) & 1023] + 0.01850666f * fVec81[(IOTA0 - 566) & 1023] + 0.02047213f * fVec81[(IOTA0 - 565) & 1023] + 0.02233686f * fVec81[(IOTA0 - 564) & 1023] + 0.02397593f * fVec81[(IOTA0 - 563) & 1023] + 0.02493095f * fVec81[(IOTA0 - 562) & 1023] + 0.02541139f * fVec81[(IOTA0 - 561) & 1023] + 0.02614199f * fVec81[(IOTA0 - 560) & 1023] + 0.02710168f * fVec81[(IOTA0 - 559) & 1023] + 0.02808494f * fVec81[(IOTA0 - 558) & 1023] + 0.02805349f * fVec81[(IOTA0 - 557) & 1023] + 0.02649867f * fVec81[(IOTA0 - 556) & 1023] + 0.02334623f * fVec81[(IOTA0 - 555) & 1023] + 0.01814049f * fVec81[(IOTA0 - 554) & 1023] + 0.0122268f * fVec81[(IOTA0 - 553) & 1023] + 0.00721957f * fVec81[(IOTA0 - 552) & 1023] + 0.00428619f * fVec81[(IOTA0 - 551) & 1023] + 0.00481601f * fVec81[(IOTA0 - 550) & 1023] + 0.0081648f * fVec81[(IOTA0 - 549) & 1023] + 0.01321822f * fVec81[(IOTA0 - 548) & 1023] + 0.01851126f * fVec81[(IOTA0 - 547) & 1023] + 0.02219456f * fVec81[(IOTA0 - 546) & 1023] + 0.02358241f * fVec81[(IOTA0 - 545) & 1023] + 0.02276572f * fVec81[(IOTA0 - 544) & 1023] + 0.02054153f * fVec81[(IOTA0 - 543) & 1023] + 0.01807017f * fVec81[(IOTA0 - 542) & 1023] + 0.0162261f * fVec81[(IOTA0 - 541) & 1023] + 0.01573233f * fVec81[(IOTA0 - 540) & 1023] + 0.01572012f * fVec81[(IOTA0 - 539) & 1023] + 0.01540987f * fVec81[(IOTA0 - 538) & 1023] + 0.01505279f * fVec81[(IOTA0 - 537) & 1023] + 0.01440042f * fVec81[(IOTA0 - 536) & 1023] + 0.01327233f * fVec81[(IOTA0 - 535) & 1023] + 0.01217704f * fVec81[(IOTA0 - 534) & 1023] + 0.01091927f * fVec81[(IOTA0 - 533) & 1023] + 0.00930451f * fVec81[(IOTA0 - 532) & 1023] + 0.00678407f * fVec81[(IOTA0 - 531) & 1023] + 0.00334131f * fVec81[(IOTA0 - 530) & 1023] + 0.00037296f * fVec81[(IOTA0 - 529) & 1023] + 0.00015726f * fVec81[(IOTA0 - 524) & 1023] + 0.00159965f * fVec81[(IOTA0 - 523) & 1023] + 0.00322879f * fVec81[(IOTA0 - 522) & 1023] + 0.00443008f * fVec81[(IOTA0 - 521) & 1023] + 0.00553806f * fVec81[(IOTA0 - 520) & 1023] + 0.00705298f * fVec81[(IOTA0 - 519) & 1023] + 0.0086167f * fVec81[(IOTA0 - 518) & 1023] + 0.01003195f * fVec81[(IOTA0 - 517) & 1023] + 0.01089553f * fVec81[(IOTA0 - 516) & 1023] + 0.01075873f * fVec81[(IOTA0 - 515) & 1023] + 0.00934258f * fVec81[(IOTA0 - 514) & 1023] + 0.00703221f * fVec81[(IOTA0 - 513) & 1023] + 0.00422004f * fVec81[(IOTA0 - 512) & 1023] + 0.00208585f * fVec81[(IOTA0 - 511) & 1023] + 0.00113672f * fVec81[(IOTA0 - 510) & 1023] + 0.00116557f * fVec81[(IOTA0 - 509) & 1023] + 0.00194334f * fVec81[(IOTA0 - 508) & 1023] + 0.00303094f * fVec81[(IOTA0 - 507) & 1023] + 0.00414917f * fVec81[(IOTA0 - 506) & 1023] + 0.00574741f * fVec81[(IOTA0 - 505) & 1023] + 0.00800977f * fVec81[(IOTA0 - 504) & 1023] + 0.01044743f * fVec81[(IOTA0 - 503) & 1023] + 0.01298324f * fVec81[(IOTA0 - 502) & 1023] + 0.01553774f * fVec81[(IOTA0 - 501) & 1023] + 0.01740571f * fVec81[(IOTA0 - 500) & 1023] + 0.01832353f * fVec81[(IOTA0 - 499) & 1023] + 0.01943997f * fVec81[(IOTA0 - 498) & 1023] + 0.02079081f * fVec81[(IOTA0 - 497) & 1023] + 0.02228729f * fVec81[(IOTA0 - 496) & 1023] + 0.0239524f * fVec81[(IOTA0 - 495) & 1023] + 0.02505364f * fVec81[(IOTA0 - 494) & 1023] + 0.02509367f * fVec81[(IOTA0 - 493) & 1023] + 0.02412662f * fVec81[(IOTA0 - 492) & 1023] + 0.02276733f * fVec81[(IOTA0 - 491) & 1023] + 0.02111627f * fVec81[(IOTA0 - 490) & 1023] + 0.02022575f * fVec81[(IOTA0 - 489) & 1023] + 0.02057186f * fVec81[(IOTA0 - 488) & 1023] + 0.0218979f * fVec81[(IOTA0 - 487) & 1023] + 0.02391046f * fVec81[(IOTA0 - 486) & 1023] + 0.02635566f * fVec81[(IOTA0 - 485) & 1023] + 0.02880191f * fVec81[(IOTA0 - 484) & 1023] + 0.03075063f * fVec81[(IOTA0 - 483) & 1023] + 0.03168274f * fVec81[(IOTA0 - 482) & 1023] + 0.03145704f * fVec81[(IOTA0 - 481) & 1023] + 0.03019989f * fVec81[(IOTA0 - 480) & 1023] + 0.02796293f * fVec81[(IOTA0 - 479) & 1023] + 0.02508271f * fVec81[(IOTA0 - 478) & 1023] + 0.02190375f * fVec81[(IOTA0 - 477) & 1023] + 0.01873922f * fVec81[(IOTA0 - 476) & 1023] + 0.01619867f * fVec81[(IOTA0 - 475) & 1023] + 0.01506618f * fVec81[(IOTA0 - 474) & 1023] + 0.01528031f * fVec81[(IOTA0 - 473) & 1023] + 0.01621152f * fVec81[(IOTA0 - 472) & 1023] + 0.01732635f * fVec81[(IOTA0 - 471) & 1023] + 0.0178118f * fVec81[(IOTA0 - 470) & 1023] + 0.0169915f * fVec81[(IOTA0 - 469) & 1023] + 0.01483326f * fVec81[(IOTA0 - 468) & 1023] + 0.01167969f * fVec81[(IOTA0 - 467) & 1023] + 0.00810705f * fVec81[(IOTA0 - 466) & 1023] + 0.00527276f * fVec81[(IOTA0 - 465) & 1023] + 0.00361124f * fVec81[(IOTA0 - 464) & 1023] + 0.0026926f * fVec81[(IOTA0 - 463) & 1023] + 0.00186901f * fVec81[(IOTA0 - 462) & 1023] + 0.00069553f * fVec81[(IOTA0 - 461) & 1023] + 0.0024902f * fVec81[(IOTA0 - 454) & 1023] + 0.00675208f * fVec81[(IOTA0 - 453) & 1023] + 0.01008894f * fVec81[(IOTA0 - 452) & 1023] + 0.01201682f * fVec81[(IOTA0 - 451) & 1023] + 0.01230769f * fVec81[(IOTA0 - 450) & 1023] + 0.01113476f * fVec81[(IOTA0 - 449) & 1023] + 0.00872375f * fVec81[(IOTA0 - 448) & 1023] + 0.0053493f * fVec81[(IOTA0 - 447) & 1023] + 0.00152912f * fVec81[(IOTA0 - 446) & 1023] + 0.00140839f * fVec81[(IOTA0 - 439) & 1023] + 0.0050049f * fVec81[(IOTA0 - 438) & 1023] + 0.00818341f * fVec81[(IOTA0 - 437) & 1023] + 0.01116617f * fVec81[(IOTA0 - 436) & 1023] + 0.01345074f * fVec81[(IOTA0 - 435) & 1023] + 0.01462169f * fVec81[(IOTA0 - 434) & 1023] + 0.01450275f * fVec81[(IOTA0 - 433) & 1023] + 0.01316767f * fVec81[(IOTA0 - 432) & 1023] + 0.01111816f * fVec81[(IOTA0 - 431) & 1023] + 0.00925527f * fVec81[(IOTA0 - 430) & 1023] + 0.00780193f * fVec81[(IOTA0 - 429) & 1023] + 0.00665097f * fVec81[(IOTA0 - 428) & 1023] + 0.00542886f * fVec81[(IOTA0 - 427) & 1023] + 0.00388614f * fVec81[(IOTA0 - 426) & 1023] + 0.0016202f * fVec81[(IOTA0 - 425) & 1023] + 0.00310708f * fVec81[(IOTA0 - 416) & 1023] + 0.00513477f * fVec81[(IOTA0 - 415) & 1023] + 0.00535003f * fVec81[(IOTA0 - 414) & 1023] + 0.00432034f * fVec81[(IOTA0 - 413) & 1023] + 0.00303005f * fVec81[(IOTA0 - 412) & 1023] + 0.00258861f * fVec81[(IOTA0 - 411) & 1023] + 0.00342186f * fVec81[(IOTA0 - 410) & 1023] + 0.00538372f * fVec81[(IOTA0 - 409) & 1023] + 0.00767854f * fVec81[(IOTA0 - 408) & 1023] + 0.0095136f * fVec81[(IOTA0 - 407) & 1023] + 0.01061552f * fVec81[(IOTA0 - 406) & 1023] + 0.01116868f * fVec81[(IOTA0 - 405) & 1023] + 0.01167687f * fVec81[(IOTA0 - 404) & 1023] + 0.01256332f * fVec81[(IOTA0 - 403) & 1023] + 0.01413349f * fVec81[(IOTA0 - 402) & 1023] + 0.01589697f * fVec81[(IOTA0 - 401) & 1023] + 0.016848f * fVec81[(IOTA0 - 400) & 1023] + 0.01632355f * fVec81[(IOTA0 - 399) & 1023] + 0.01428123f * fVec81[(IOTA0 - 398) & 1023] + 0.01123682f * fVec81[(IOTA0 - 397) & 1023] + 0.00826306f * fVec81[(IOTA0 - 396) & 1023] + 0.00647082f * fVec81[(IOTA0 - 395) & 1023] + 0.00597591f * fVec81[(IOTA0 - 394) & 1023] + 0.00647749f * fVec81[(IOTA0 - 393) & 1023] + 0.0077014f * fVec81[(IOTA0 - 392) & 1023] + 0.00872529f * fVec81[(IOTA0 - 391) & 1023] + 0.00882347f * fVec81[(IOTA0 - 390) & 1023] + 0.00778315f * fVec81[(IOTA0 - 389) & 1023] + 0.00572341f * fVec81[(IOTA0 - 388) & 1023] + 0.00290907f * fVec81[(IOTA0 - 387) & 1023] + 0.00366397f * fVec81[(IOTA0 - 261) & 1023] + 0.00882564f * fVec81[(IOTA0 - 260) & 1023] + 0.01178197f * fVec81[(IOTA0 - 259) & 1023] + 0.01352711f * fVec81[(IOTA0 - 258) & 1023] + 0.01476399f * fVec81[(IOTA0 - 257) & 1023] + 0.01538108f * fVec81[(IOTA0 - 256) & 1023] + 0.01589989f * fVec81[(IOTA0 - 255) & 1023] + 0.01596954f * fVec81[(IOTA0 - 254) & 1023] + 0.01439774f * fVec81[(IOTA0 - 253) & 1023] + 0.01118128f * fVec81[(IOTA0 - 252) & 1023] + 0.00669212f * fVec81[(IOTA0 - 251) & 1023] + 0.0013781f * fVec81[(IOTA0 - 250) & 1023] + 0.00217308f * fVec81[(IOTA0 - 200) & 1023] + 0.00020242f * fVec81[(IOTA0 - 199) & 1023] + 0.00048931f * fVec81[(IOTA0 - 188) & 1023] + 0.00254402f * fVec81[(IOTA0 - 187) & 1023] + 0.00569348f * fVec81[(IOTA0 - 186) & 1023] + 0.01008962f * fVec81[(IOTA0 - 185) & 1023] + 0.01629391f * fVec81[(IOTA0 - 184) & 1023] + 0.02348773f * fVec81[(IOTA0 - 183) & 1023] + 0.03101398f * fVec81[(IOTA0 - 182) & 1023] + 0.03625652f * fVec81[(IOTA0 - 181) & 1023] + 0.03969939f * fVec81[(IOTA0 - 180) & 1023] + 0.04119597f * fVec81[(IOTA0 - 179) & 1023] + 0.04354497f * fVec81[(IOTA0 - 178) & 1023] + 0.04609942f * fVec81[(IOTA0 - 177) & 1023] + 0.0501165f * fVec81[(IOTA0 - 176) & 1023] + 0.05172953f * fVec81[(IOTA0 - 175) & 1023] + 0.05055175f * fVec81[(IOTA0 - 174) & 1023] + 0.04537446f * fVec81[(IOTA0 - 173) & 1023] + 0.03851223f * fVec81[(IOTA0 - 172) & 1023] + 0.03118234f * fVec81[(IOTA0 - 171) & 1023] + 0.02726157f * fVec81[(IOTA0 - 170) & 1023] + 0.02721965f * fVec81[(IOTA0 - 169) & 1023] + 0.03058608f * fVec81[(IOTA0 - 168) & 1023] + 0.03512677f * fVec81[(IOTA0 - 167) & 1023] + 0.03834483f * fVec81[(IOTA0 - 166) & 1023] + 0.0376997f * fVec81[(IOTA0 - 165) & 1023] + 0.03327315f * fVec81[(IOTA0 - 164) & 1023] + 0.02662594f * fVec81[(IOTA0 - 163) & 1023] + 0.01996929f * fVec81[(IOTA0 - 162) & 1023] + 0.01345075f * fVec81[(IOTA0 - 161) & 1023] + 0.00887638f * fVec81[(IOTA0 - 160) & 1023] + 0.00645785f * fVec81[(IOTA0 - 159) & 1023] + 0.00675816f * fVec81[(IOTA0 - 158) & 1023] + 0.00768286f * fVec81[(IOTA0 - 157) & 1023] + 0.00922429f * fVec81[(IOTA0 - 156) & 1023] + 0.01096199f * fVec81[(IOTA0 - 155) & 1023] + 0.01302164f * fVec81[(IOTA0 - 154) & 1023] + 0.01399632f * fVec81[(IOTA0 - 153) & 1023] + 0.01391811f * fVec81[(IOTA0 - 152) & 1023] + 0.0124102f * fVec81[(IOTA0 - 151) & 1023] + 0.00896629f * fVec81[(IOTA0 - 150) & 1023] + 0.00458128f * fVec81[(IOTA0 - 149) & 1023] + 0.00109997f * fVec81[(IOTA0 - 148) & 1023] + 0.0018391f * fVec81[(IOTA0 - 147) & 1023] + 0.0056709f * fVec81[(IOTA0 - 146) & 1023] + 0.0133009f * fVec81[(IOTA0 - 145) & 1023] + 0.02245729f * fVec81[(IOTA0 - 144) & 1023] + 0.03225203f * fVec81[(IOTA0 - 143) & 1023] + 0.03695855f * fVec81[(IOTA0 - 142) & 1023] + 0.03556176f * fVec81[(IOTA0 - 141) & 1023] + 0.02955926f * fVec81[(IOTA0 - 140) & 1023] + 0.01744731f * fVec81[(IOTA0 - 139) & 1023] + 0.00075294f * fVec81[(IOTA0 - 138) & 1023] + 0.01057233f * fVec81[(IOTA0 - 133) & 1023] + 0.04071832f * fVec81[(IOTA0 - 132) & 1023] + 0.06802493f * fVec81[(IOTA0 - 131) & 1023] + 0.08378442f * fVec81[(IOTA0 - 130) & 1023] + 0.0839572f * fVec81[(IOTA0 - 129) & 1023] + 0.07084565f * fVec81[(IOTA0 - 128) & 1023] + 0.05085025f * fVec81[(IOTA0 - 127) & 1023] + 0.02621219f * fVec81[(IOTA0 - 126) & 1023] + 0.00741142f * fVec81[(IOTA0 - 125) & 1023] + 0.00126677f * fVec81[(IOTA0 - 121) & 1023] + 0.0188235f * fVec81[(IOTA0 - 120) & 1023] + 0.0433865f * fVec81[(IOTA0 - 119) & 1023] + 0.06846095f * fVec81[(IOTA0 - 118) & 1023] + 0.08890308f * fVec81[(IOTA0 - 117) & 1023] + 0.09720974f * fVec81[(IOTA0 - 116) & 1023] + 0.09087553f * fVec81[(IOTA0 - 115) & 1023] + 0.06918254f * fVec81[(IOTA0 - 114) & 1023] + 0.03796919f * fVec81[(IOTA0 - 113) & 1023] + 0.00544651f * fVec81[(IOTA0 - 112) & 1023] + 8.395e-05f * fVec81[(IOTA0 - 109) & 1023] + 0.02467561f * fVec81[(IOTA0 - 108) & 1023] + 0.05116061f * fVec81[(IOTA0 - 107) & 1023] + 0.06195615f * fVec81[(IOTA0 - 106) & 1023] + 0.06332258f * fVec81[(IOTA0 - 105) & 1023] + 0.06080219f * fVec81[(IOTA0 - 104) & 1023] + 0.05672739f * fVec81[(IOTA0 - 103) & 1023] + 0.05929116f * fVec81[(IOTA0 - 102) & 1023] + 0.07713243f * fVec81[(IOTA0 - 101) & 1023] + 0.10133039f * fVec81[(IOTA0 - 100) & 1023] + 0.12579367f * fVec81[(IOTA0 - 99) & 1023] + 0.1435711f * fVec81[(IOTA0 - 98) & 1023] + 0.15240477f * fVec81[(IOTA0 - 97) & 1023] + 0.1515241f * fVec81[(IOTA0 - 96) & 1023] + 0.14265208f * fVec81[(IOTA0 - 95) & 1023] + 0.12509473f * fVec81[(IOTA0 - 94) & 1023] + 0.10194603f * fVec81[(IOTA0 - 93) & 1023] + 0.06729344f * fVec81[(IOTA0 - 92) & 1023] + 0.03326318f * fVec81[(IOTA0 - 91) & 1023] + 0.01954493f * fVec81[(IOTA0 - 85) & 1023] + 0.0456923f * fVec81[(IOTA0 - 84) & 1023] + 0.0687727f * fVec81[(IOTA0 - 83) & 1023] + 0.08384955f * fVec81[(IOTA0 - 82) & 1023] + 0.08937661f * fVec81[(IOTA0 - 81) & 1023] + 0.09472341f * fVec81[(IOTA0 - 80) & 1023] + 0.09770898f * fVec81[(IOTA0 - 79) & 1023] + 0.10457174f * fVec81[(IOTA0 - 78) & 1023] + 0.09838097f * fVec81[(IOTA0 - 77) & 1023] + 0.08434149f * fVec81[(IOTA0 - 76) & 1023] + 0.05335669f * fVec81[(IOTA0 - 75) & 1023] + 0.01910052f * fVec81[(IOTA0 - 74) & 1023] + 0.02053749f * fVec81[(IOTA0 - 70) & 1023] + 0.07372529f * fVec81[(IOTA0 - 69) & 1023] + 0.10944164f * fVec81[(IOTA0 - 68) & 1023] + 0.12976794f * fVec81[(IOTA0 - 67) & 1023] + 0.10832296f * fVec81[(IOTA0 - 66) & 1023] + 0.06456775f * fVec81[(IOTA0 - 65) & 1023] + 0.00717099f * fVec81[(IOTA0 - 64) & 1023] + 0.02950917f * fVec81[(IOTA0 - 59) & 1023] + 0.06891695f * fVec81[(IOTA0 - 58) & 1023] + 0.09010027f * fVec81[(IOTA0 - 57) & 1023] + 0.09156888f * fVec81[(IOTA0 - 56) & 1023] + 0.08361053f * fVec81[(IOTA0 - 55) & 1023] + 0.08925528f * fVec81[(IOTA0 - 54) & 1023] + 0.10549672f * fVec81[(IOTA0 - 53) & 1023] + 0.13600606f * fVec81[(IOTA0 - 52) & 1023] + 0.15909937f * fVec81[(IOTA0 - 51) & 1023] + 0.15288837f * fVec81[(IOTA0 - 50) & 1023] + 0.11034634f * fVec81[(IOTA0 - 49) & 1023] + 0.03756165f * fVec81[(IOTA0 - 48) & 1023] + 0.02744843f * fVec81[(IOTA0 - 41) & 1023] + 0.06116334f * fVec81[(IOTA0 - 40) & 1023] + 0.09871148f * fVec81[(IOTA0 - 39) & 1023] + 0.15555885f * fVec81[(IOTA0 - 38) & 1023] + 0.20067479f * fVec81[(IOTA0 - 37) & 1023] + 0.23618574f * fVec81[(IOTA0 - 36) & 1023] + 0.23818648f * fVec81[(IOTA0 - 35) & 1023] + 0.19160254f * fVec81[(IOTA0 - 34) & 1023] + 0.09596425f * fVec81[(IOTA0 - 33) & 1023] + 0.02418848f * fVec81[(IOTA0 - 27) & 1023] + 0.03165585f * fVec81[(IOTA0 - 26) & 1023] + 0.02742866f * fVec81[(IOTA0 - 19) & 1023] + 0.03178342f * fVec81[(IOTA0 - 18) & 1023] + 0.12636417f * fVec81[(IOTA0 - 12) & 1023] + 0.37115902f * fVec81[(IOTA0 - 11) & 1023] + 0.54805017f * fVec81[(IOTA0 - 10) & 1023] + 0.5322162f * fVec81[(IOTA0 - 9) & 1023] + 0.00034097f * fVec83[(IOTA0 - 371) & 1023] + 0.00092305f * fVec83[(IOTA0 - 370) & 1023] + 0.00154838f * fVec83[(IOTA0 - 369) & 1023] + 0.00186006f * fVec83[(IOTA0 - 368) & 1023] + 0.00175069f * fVec83[(IOTA0 - 367) & 1023] + 0.00152119f * fVec83[(IOTA0 - 366) & 1023] + 0.00132507f * fVec83[(IOTA0 - 365) & 1023] + 0.00094776f * fVec83[(IOTA0 - 364) & 1023] + 0.00032515f * fVec83[(IOTA0 - 363) & 1023] + 0.00085372f * fVec83[(IOTA0 - 360) & 1023] + 0.00229483f * fVec83[(IOTA0 - 359) & 1023] + 0.00425252f * fVec83[(IOTA0 - 358) & 1023] + 0.00665215f * fVec83[(IOTA0 - 357) & 1023] + 0.00928931f * fVec83[(IOTA0 - 356) & 1023] + 0.01202039f * fVec83[(IOTA0 - 355) & 1023] + 0.01495686f * fVec83[(IOTA0 - 354) & 1023] + 0.01818339f * fVec83[(IOTA0 - 353) & 1023] + 0.02128559f * fVec83[(IOTA0 - 352) & 1023] + 0.02351418f * fVec83[(IOTA0 - 351) & 1023] + 0.02442116f * fVec83[(IOTA0 - 350) & 1023] + 0.02412493f * fVec83[(IOTA0 - 349) & 1023] + 0.0229515f * fVec83[(IOTA0 - 348) & 1023] + 0.02104077f * fVec83[(IOTA0 - 347) & 1023] + 0.01850903f * fVec83[(IOTA0 - 346) & 1023] + 0.01585964f * fVec83[(IOTA0 - 345) & 1023] + 0.01374055f * fVec83[(IOTA0 - 344) & 1023] + 0.01236593f * fVec83[(IOTA0 - 343) & 1023] + 0.01168016f * fVec83[(IOTA0 - 342) & 1023] + 0.0119471f * fVec83[(IOTA0 - 341) & 1023] + 0.01347299f * fVec83[(IOTA0 - 340) & 1023] + 0.01591081f * fVec83[(IOTA0 - 339) & 1023] + 0.01842636f * fVec83[(IOTA0 - 338) & 1023] + 0.02047624f * fVec83[(IOTA0 - 337) & 1023] + 0.02184864f * fVec83[(IOTA0 - 336) & 1023] + 0.02201276f * fVec83[(IOTA0 - 335) & 1023] + 0.02021078f * fVec83[(IOTA0 - 334) & 1023] + 0.01640267f * fVec83[(IOTA0 - 333) & 1023] + 0.00550789f * fVec83[(IOTA0 - 331) & 1023] + 0.00297479f * fVec83[(IOTA0 - 315) & 1023] + 0.00681812f * fVec83[(IOTA0 - 314) & 1023] + 0.01017207f * fVec83[(IOTA0 - 313) & 1023] + 0.01269367f * fVec83[(IOTA0 - 312) & 1023] + 0.01444115f * fVec83[(IOTA0 - 311) & 1023] + 0.01530006f * fVec83[(IOTA0 - 310) & 1023] + 0.01494364f * fVec83[(IOTA0 - 309) & 1023] + 0.01343517f * fVec83[(IOTA0 - 308) & 1023] + 0.01131806f * fVec83[(IOTA0 - 307) & 1023] + 0.00884972f * fVec83[(IOTA0 - 306) & 1023] + 0.00575221f * fVec83[(IOTA0 - 305) & 1023] + 0.0020332f * fVec83[(IOTA0 - 304) & 1023] + 0.00115675f * fVec83[(IOTA0 - 292) & 1023] + 0.00157656f * fVec83[(IOTA0 - 291) & 1023] + 0.00146435f * fVec83[(IOTA0 - 290) & 1023] + 0.00098027f * fVec83[(IOTA0 - 289) & 1023] + 5.93e-06f * fVec83[(IOTA0 - 288) & 1023] + 0.0007986f * fVec83[(IOTA0 - 281) & 1023] + 0.00208066f * fVec83[(IOTA0 - 280) & 1023] + 0.00222476f * fVec83[(IOTA0 - 279) & 1023] + 0.00154307f * fVec83[(IOTA0 - 278) & 1023] + 0.00072074f * fVec83[(IOTA0 - 277) & 1023] + 0.00082875f * fVec83[(IOTA0 - 273) & 1023] + 0.00370146f * fVec83[(IOTA0 - 272) & 1023] + 0.00785512f * fVec83[(IOTA0 - 271) & 1023] + 0.01326426f * fVec83[(IOTA0 - 270) & 1023] + 0.0203209f * fVec83[(IOTA0 - 269) & 1023] + 0.02886796f * fVec83[(IOTA0 - 268) & 1023] + 0.03774956f * fVec83[(IOTA0 - 267) & 1023] + 0.04584134f * fVec83[(IOTA0 - 266) & 1023] + 0.05270259f * fVec83[(IOTA0 - 265) & 1023] + 0.05785283f * fVec83[(IOTA0 - 264) & 1023] + 0.06046515f * fVec83[(IOTA0 - 263) & 1023] + 0.06036591f * fVec83[(IOTA0 - 262) & 1023] + 0.05861658f * fVec83[(IOTA0 - 261) & 1023] + 0.05627837f * fVec83[(IOTA0 - 260) & 1023] + 0.05322597f * fVec83[(IOTA0 - 259) & 1023] + 0.04903574f * fVec83[(IOTA0 - 258) & 1023] + 0.04426552f * fVec83[(IOTA0 - 257) & 1023] + 0.03979883f * fVec83[(IOTA0 - 256) & 1023] + 0.03570573f * fVec83[(IOTA0 - 255) & 1023] + 0.03170483f * fVec83[(IOTA0 - 254) & 1023] + 0.02817169f * fVec83[(IOTA0 - 253) & 1023] + 0.02558359f * fVec83[(IOTA0 - 252) & 1023] + 0.02327159f * fVec83[(IOTA0 - 251) & 1023] + 0.02002355f * fVec83[(IOTA0 - 250) & 1023] + 0.01598236f * fVec83[(IOTA0 - 249) & 1023] + 0.01237309f * fVec83[(IOTA0 - 248) & 1023] + 0.00974101f * fVec83[(IOTA0 - 247) & 1023] + 0.00771349f * fVec83[(IOTA0 - 246) & 1023] + 0.00590966f * fVec83[(IOTA0 - 245) & 1023] + 0.00434027f * fVec83[(IOTA0 - 244) & 1023] + 0.00320997f * fVec83[(IOTA0 - 243) & 1023] + 0.00277594f * fVec83[(IOTA0 - 242) & 1023] + 0.00361656f * fVec83[(IOTA0 - 241) & 1023] + 0.00663324f * fVec83[(IOTA0 - 240) & 1023] + 0.01192325f * fVec83[(IOTA0 - 239) & 1023] + 0.01842772f * fVec83[(IOTA0 - 238) & 1023] + 0.02509358f * fVec83[(IOTA0 - 237) & 1023] + 0.03174438f * fVec83[(IOTA0 - 236) & 1023] + 0.03871203f * fVec83[(IOTA0 - 235) & 1023] + 0.04580488f * fVec83[(IOTA0 - 234) & 1023] + 0.0520974f * fVec83[(IOTA0 - 233) & 1023] + 0.0572244f * fVec83[(IOTA0 - 232) & 1023] + 0.06201019f * fVec83[(IOTA0 - 231) & 1023] + 0.06684516f * fVec83[(IOTA0 - 230) & 1023] + 0.07084867f * fVec83[(IOTA0 - 229) & 1023] + 0.07329068f * fVec83[(IOTA0 - 228) & 1023] + 0.07432137f * fVec83[(IOTA0 - 227) & 1023] + 0.07399017f * fVec83[(IOTA0 - 226) & 1023] + 0.0719336f * fVec83[(IOTA0 - 225) & 1023] + 0.06827465f * fVec83[(IOTA0 - 224) & 1023] + 0.06397728f * fVec83[(IOTA0 - 223) & 1023] + 0.05978556f * fVec83[(IOTA0 - 222) & 1023] + 0.05534741f * fVec83[(IOTA0 - 221) & 1023] + 0.05038107f * fVec83[(IOTA0 - 220) & 1023] + 0.04557848f * fVec83[(IOTA0 - 219) & 1023] + 0.04141865f * fVec83[(IOTA0 - 218) & 1023] + 0.03741119f * fVec83[(IOTA0 - 217) & 1023] + 0.03332563f * fVec83[(IOTA0 - 216) & 1023] + 0.03006289f * fVec83[(IOTA0 - 215) & 1023] + 0.02879888f * fVec83[(IOTA0 - 214) & 1023] + 0.03001778f * fVec83[(IOTA0 - 213) & 1023] + 0.03346293f * fVec83[(IOTA0 - 212) & 1023] + 0.03855434f * fVec83[(IOTA0 - 211) & 1023] + 0.04464493f * fVec83[(IOTA0 - 210) & 1023] + 0.05129176f * fVec83[(IOTA0 - 209) & 1023] + 0.05798791f * fVec83[(IOTA0 - 208) & 1023] + 0.06379215f * fVec83[(IOTA0 - 207) & 1023] + 0.06746679f * fVec83[(IOTA0 - 206) & 1023] + 0.06791651f * fVec83[(IOTA0 - 205) & 1023] + 0.06483507f * fVec83[(IOTA0 - 204) & 1023] + 0.05930247f * fVec83[(IOTA0 - 203) & 1023] + 0.05351553f * fVec83[(IOTA0 - 202) & 1023] + 0.0493526f * fVec83[(IOTA0 - 201) & 1023] + 0.04741642f * fVec83[(IOTA0 - 200) & 1023] + 0.04776196f * fVec83[(IOTA0 - 199) & 1023] + 0.05106956f * fVec83[(IOTA0 - 198) & 1023] + 0.05802856f * fVec83[(IOTA0 - 197) & 1023] + 0.06829369f * fVec83[(IOTA0 - 196) & 1023] + 0.08101405f * fVec83[(IOTA0 - 195) & 1023] + 0.09541442f * fVec83[(IOTA0 - 194) & 1023] + 0.11009482f * fVec83[(IOTA0 - 193) & 1023] + 0.12276551f * fVec83[(IOTA0 - 192) & 1023] + 0.13162747f * fVec83[(IOTA0 - 191) & 1023] + 0.13666512f * fVec83[(IOTA0 - 190) & 1023] + 0.13851357f * fVec83[(IOTA0 - 189) & 1023] + 0.13655995f * fVec83[(IOTA0 - 188) & 1023] + 0.1298062f * fVec83[(IOTA0 - 187) & 1023] + 0.11912251f * fVec83[(IOTA0 - 186) & 1023] + 0.1067415f * fVec83[(IOTA0 - 185) & 1023] + 0.09405747f * fVec83[(IOTA0 - 184) & 1023] + 0.08169649f * fVec83[(IOTA0 - 183) & 1023] + 0.07075524f * fVec83[(IOTA0 - 182) & 1023] + 0.06182531f * fVec83[(IOTA0 - 181) & 1023] + 0.05372735f * fVec83[(IOTA0 - 180) & 1023] + 0.04501197f * fVec83[(IOTA0 - 179) & 1023] + 0.03627607f * fVec83[(IOTA0 - 178) & 1023] + 0.0291978f * fVec83[(IOTA0 - 177) & 1023] + 0.02344201f * fVec83[(IOTA0 - 176) & 1023] + 0.01691394f * fVec83[(IOTA0 - 175) & 1023] + 0.00877767f * fVec83[(IOTA0 - 174) & 1023] + 0.00039065f * fVec83[(IOTA0 - 173) & 1023] + 0.00427032f * fVec83[(IOTA0 - 168) & 1023] + 0.01786442f * fVec83[(IOTA0 - 167) & 1023] + 0.03407715f * fVec83[(IOTA0 - 166) & 1023] + 0.05257555f * fVec83[(IOTA0 - 165) & 1023] + 0.0725252f * fVec83[(IOTA0 - 164) & 1023] + 0.09202155f * fVec83[(IOTA0 - 163) & 1023] + 0.10845894f * fVec83[(IOTA0 - 162) & 1023] + 0.11996275f * fVec83[(IOTA0 - 161) & 1023] + 0.12675343f * fVec83[(IOTA0 - 160) & 1023] + 0.12995587f * fVec83[(IOTA0 - 159) & 1023] + 0.12933473f * fVec83[(IOTA0 - 158) & 1023] + 0.12412914f * fVec83[(IOTA0 - 157) & 1023] + 0.11526731f * fVec83[(IOTA0 - 156) & 1023] + 0.10401351f * fVec83[(IOTA0 - 155) & 1023] + 0.08928182f * fVec83[(IOTA0 - 154) & 1023] + 0.06935478f * fVec83[(IOTA0 - 153) & 1023] + 0.04500913f * fVec83[(IOTA0 - 152) & 1023] + 0.01843142f * fVec83[(IOTA0 - 151) & 1023] + 0.03593337f * fVec83[(IOTA0 - 140) & 1023] + 0.0774971f * fVec83[(IOTA0 - 139) & 1023] + 0.1156105f * fVec83[(IOTA0 - 138) & 1023] + 0.14481065f * fVec83[(IOTA0 - 137) & 1023] + 0.16340001f * fVec83[(IOTA0 - 136) & 1023] + 0.1736618f * fVec83[(IOTA0 - 135) & 1023] + 0.17665488f * fVec83[(IOTA0 - 134) & 1023] + 0.17117059f * fVec83[(IOTA0 - 133) & 1023] + 0.15844356f * fVec83[(IOTA0 - 132) & 1023] + 0.1437791f * fVec83[(IOTA0 - 131) & 1023] + 0.13164192f * fVec83[(IOTA0 - 130) & 1023] + 0.12189116f * fVec83[(IOTA0 - 129) & 1023] + 0.11292947f * fVec83[(IOTA0 - 128) & 1023] + 0.1056049f * fVec83[(IOTA0 - 127) & 1023] + 0.10111634f * fVec83[(IOTA0 - 126) & 1023] + 0.09760445f * fVec83[(IOTA0 - 125) & 1023] + 0.09224017f * fVec83[(IOTA0 - 124) & 1023] + 0.08464149f * fVec83[(IOTA0 - 123) & 1023] + 0.07503679f * fVec83[(IOTA0 - 122) & 1023] + 0.06073394f * fVec83[(IOTA0 - 121) & 1023] + 0.03860142f * fVec83[(IOTA0 - 120) & 1023] + 0.01120115f * fVec83[(IOTA0 - 119) & 1023] + 0.01960708f * fVec83[(IOTA0 - 111) & 1023] + 0.058034f * fVec83[(IOTA0 - 110) & 1023] + 0.09822422f * fVec83[(IOTA0 - 109) & 1023] + 0.1308039f * fVec83[(IOTA0 - 108) & 1023] + 0.15025255f * fVec83[(IOTA0 - 107) & 1023] + 0.15919223f * fVec83[(IOTA0 - 106) & 1023] + 0.16170976f * fVec83[(IOTA0 - 105) & 1023] + 0.1574502f * fVec83[(IOTA0 - 104) & 1023] + 0.1438137f * fVec83[(IOTA0 - 103) & 1023] + 0.12048279f * fVec83[(IOTA0 - 102) & 1023] + 0.09059601f * fVec83[(IOTA0 - 101) & 1023] + 0.057783f * fVec83[(IOTA0 - 100) & 1023] + 0.02447999f * fVec83[(IOTA0 - 99) & 1023] + 0.00678784f * fVec83[(IOTA0 - 91) & 1023] + 0.01268799f * fVec83[(IOTA0 - 90) & 1023] + 0.01199468f * fVec83[(IOTA0 - 89) & 1023] + 0.01101862f * fVec83[(IOTA0 - 88) & 1023] + 0.01370496f * fVec83[(IOTA0 - 87) & 1023] + 0.01852485f * fVec83[(IOTA0 - 86) & 1023] + 0.02512176f * fVec83[(IOTA0 - 85) & 1023] + 0.03678178f * fVec83[(IOTA0 - 84) & 1023] + 0.05391162f * fVec83[(IOTA0 - 83) & 1023] + 0.06945958f * fVec83[(IOTA0 - 82) & 1023] + 0.07742666f * fVec83[(IOTA0 - 81) & 1023] + 0.08191585f * fVec83[(IOTA0 - 80) & 1023] + 0.08941319f * fVec83[(IOTA0 - 79) & 1023] + 0.09912861f * fVec83[(IOTA0 - 78) & 1023] + 0.10807373f * fVec83[(IOTA0 - 77) & 1023] + 0.11643937f * fVec83[(IOTA0 - 76) & 1023] + 0.12281223f * fVec83[(IOTA0 - 75) & 1023] + 0.12155835f * fVec83[(IOTA0 - 74) & 1023] + 0.10902546f * fVec83[(IOTA0 - 73) & 1023] + 0.08935362f * fVec83[(IOTA0 - 72) & 1023] + 0.06866691f * fVec83[(IOTA0 - 71) & 1023] + 0.04664377f * fVec83[(IOTA0 - 70) & 1023] + 0.02142969f * fVec83[(IOTA0 - 69) & 1023] + 0.02509309f * fVec83[(IOTA0 - 56) & 1023] + 0.05758749f * fVec83[(IOTA0 - 55) & 1023] + 0.09096491f * fVec83[(IOTA0 - 54) & 1023] + 0.11887632f * fVec83[(IOTA0 - 53) & 1023] + 0.13756418f * fVec83[(IOTA0 - 52) & 1023] + 0.14520834f * fVec83[(IOTA0 - 51) & 1023] + 0.1383964f * fVec83[(IOTA0 - 50) & 1023] + 0.11413911f * fVec83[(IOTA0 - 49) & 1023] + 0.07522576f * fVec83[(IOTA0 - 48) & 1023] + 0.02883793f * fVec83[(IOTA0 - 47) & 1023] + 0.10178962f * fVec83[(IOTA0 - 26) & 1023] + 0.25611696f * fVec83[(IOTA0 - 25) & 1023] + 0.41216832f * fVec83[(IOTA0 - 24) & 1023] + 0.5485765f * fVec83[(IOTA0 - 23) & 1023] + 0.64691985f * fVec83[(IOTA0 - 22) & 1023] + 0.6878026f * fVec83[(IOTA0 - 21) & 1023] + 0.6583259f * fVec83[(IOTA0 - 20) & 1023] + 0.55967754f * fVec83[(IOTA0 - 19) & 1023] + 0.4009323f * fVec83[(IOTA0 - 18) & 1023] + 0.19070241f * fVec83[(IOTA0 - 17) & 1023] + 0.00057187f * fVec82[(IOTA0 - 623) & 1023] + 0.0006728f * fVec82[(IOTA0 - 622) & 1023] + 0.00124064f * fVec82[(IOTA0 - 621) & 1023] + 0.00126374f * fVec82[(IOTA0 - 620) & 1023] + 9.678e-05f * fVec82[(IOTA0 - 619) & 1023] + 1.686e-05f * fVec82[(IOTA0 - 617) & 1023] + 0.00518313f * fVec82[(IOTA0 - 601) & 1023] + 0.00815844f * fVec82[(IOTA0 - 600) & 1023] + 0.01032837f * fVec82[(IOTA0 - 599) & 1023] + 0.01205046f * (fVec82[(IOTA0 - 371) & 1023] + fVec82[(IOTA0 - 598) & 1023]) + 0.01275491f * fVec82[(IOTA0 - 597) & 1023] + 0.01217669f * fVec82[(IOTA0 - 596) & 1023] + 0.01172884f * fVec82[(IOTA0 - 595) & 1023] + 0.01189491f * fVec82[(IOTA0 - 594) & 1023] + 0.01281173f * fVec82[(IOTA0 - 593) & 1023] + 0.01361064f * fVec82[(IOTA0 - 592) & 1023] + 0.01348453f * fVec82[(IOTA0 - 591) & 1023] + 0.01330161f * fVec82[(IOTA0 - 590) & 1023] + 0.01225232f * fVec82[(IOTA0 - 589) & 1023] + 0.00977324f * fVec82[(IOTA0 - 588) & 1023] + 0.00625758f * fVec82[(IOTA0 - 587) & 1023] + 0.00249167f * fVec82[(IOTA0 - 586) & 1023] + 0.00011987f * fVec82[(IOTA0 - 585) & 1023] + 0.00028386f * fVec82[(IOTA0 - 581) & 1023] + 0.00311828f * fVec82[(IOTA0 - 580) & 1023] + 0.00548165f * fVec82[(IOTA0 - 579) & 1023] + 0.00685059f * fVec82[(IOTA0 - 578) & 1023] + 0.00732361f * fVec82[(IOTA0 - 577) & 1023] + 0.00666131f * fVec82[(IOTA0 - 576) & 1023] + 0.0048488f * fVec82[(IOTA0 - 575) & 1023] + 0.00220366f * fVec82[(IOTA0 - 574) & 1023] + 0.00102607f * fVec82[(IOTA0 - 564) & 1023] + 0.00245171f * fVec82[(IOTA0 - 563) & 1023] + 0.00343794f * fVec82[(IOTA0 - 562) & 1023] + 0.00444923f * fVec82[(IOTA0 - 561) & 1023] + 0.00593804f * fVec82[(IOTA0 - 560) & 1023] + 0.00687784f * fVec82[(IOTA0 - 559) & 1023] + 0.00686953f * fVec82[(IOTA0 - 558) & 1023] + 0.00614614f * fVec82[(IOTA0 - 556) & 1023] + 0.00477096f * fVec82[(IOTA0 - 555) & 1023] + 0.00315616f * fVec82[(IOTA0 - 554) & 1023] + 0.0015581f * fVec82[(IOTA0 - 553) & 1023] + 0.00063504f * fVec82[(IOTA0 - 552) & 1023] + 0.00019771f * fVec82[(IOTA0 - 551) & 1023] + 0.00010301f * fVec82[(IOTA0 - 550) & 1023] + 0.0012553f * fVec82[(IOTA0 - 549) & 1023] + 0.00438202f * fVec82[(IOTA0 - 548) & 1023] + 0.00785356f * fVec82[(IOTA0 - 547) & 1023] + 0.01107278f * fVec82[(IOTA0 - 546) & 1023] + 0.01456107f * fVec82[(IOTA0 - 545) & 1023] + 0.01658815f * fVec82[(IOTA0 - 544) & 1023] + 0.01753638f * fVec82[(IOTA0 - 543) & 1023] + 0.01812511f * fVec82[(IOTA0 - 542) & 1023] + 0.01658595f * fVec82[(IOTA0 - 541) & 1023] + 0.01379576f * fVec82[(IOTA0 - 540) & 1023] + 0.01106007f * fVec82[(IOTA0 - 539) & 1023] + 0.0087787f * fVec82[(IOTA0 - 538) & 1023] + 0.00726899f * fVec82[(IOTA0 - 537) & 1023] + 0.00609775f * fVec82[(IOTA0 - 536) & 1023] + 0.00584542f * fVec82[(IOTA0 - 535) & 1023] + 0.00522944f * fVec82[(IOTA0 - 534) & 1023] + 0.00328434f * fVec82[(IOTA0 - 533) & 1023] + 0.00104708f * fVec82[(IOTA0 - 531) & 1023] + 0.00268303f * fVec82[(IOTA0 - 509) & 1023] + 0.00542495f * fVec82[(IOTA0 - 508) & 1023] + 0.0077442f * fVec82[(IOTA0 - 507) & 1023] + 0.00967866f * fVec82[(IOTA0 - 506) & 1023] + 0.01070901f * fVec82[(IOTA0 - 505) & 1023] + 0.01067956f * fVec82[(IOTA0 - 504) & 1023] + 0.01162156f * fVec82[(IOTA0 - 503) & 1023] + 0.01393445f * fVec82[(IOTA0 - 502) & 1023] + 0.01567976f * fVec82[(IOTA0 - 501) & 1023] + 0.01632311f * fVec82[(IOTA0 - 500) & 1023] + 0.01656494f * fVec82[(IOTA0 - 499) & 1023] + 0.01659231f * fVec82[(IOTA0 - 498) & 1023] + 0.01527187f * fVec82[(IOTA0 - 497) & 1023] + 0.01263711f * fVec82[(IOTA0 - 496) & 1023] + 0.00863781f * fVec82[(IOTA0 - 495) & 1023] + 0.00308248f * fVec82[(IOTA0 - 494) & 1023] + 0.00089789f * fVec82[(IOTA0 - 480) & 1023] + 0.00135208f * (fVec82[(IOTA0 - 369) & 1023] + fVec82[(IOTA0 - 479) & 1023]) + 0.00148662f * fVec82[(IOTA0 - 478) & 1023] + 0.00100713f * fVec82[(IOTA0 - 477) & 1023] + 0.00084742f * (fVec82[(IOTA0 - 387) & 1023] - fVec82[(IOTA0 - 463) & 1023]) + 0.00398244f * fVec82[(IOTA0 - 462) & 1023] + 0.00801547f * fVec82[(IOTA0 - 461) & 1023] + 0.00999197f * fVec82[(IOTA0 - 460) & 1023] + 0.00997511f * fVec82[(IOTA0 - 459) & 1023] + 0.00814793f * fVec82[(IOTA0 - 458) & 1023] + 0.00630597f * fVec82[(IOTA0 - 457) & 1023] + 0.00549008f * fVec82[(IOTA0 - 456) & 1023] + 0.00367964f * fVec82[(IOTA0 - 455) & 1023] + 0.0018251f * fVec82[(IOTA0 - 454) & 1023] + 0.00186713f * fVec82[(IOTA0 - 453) & 1023] + 0.00121327f * fVec82[(IOTA0 - 452) & 1023] + 0.00072547f * fVec82[(IOTA0 - 446) & 1023] + 0.00306574f * fVec82[(IOTA0 - 445) & 1023] + 0.00649941f * fVec82[(IOTA0 - 444) & 1023] + 0.00985743f * fVec82[(IOTA0 - 443) & 1023] + 0.0120358f * fVec82[(IOTA0 - 442) & 1023] + 0.0134425f * fVec82[(IOTA0 - 441) & 1023] + 0.01403759f * fVec82[(IOTA0 - 440) & 1023] + 0.01353085f * fVec82[(IOTA0 - 439) & 1023] + 0.01248156f * fVec82[(IOTA0 - 438) & 1023] + 0.01154372f * fVec82[(IOTA0 - 437) & 1023] + 0.0114638f * fVec82[(IOTA0 - 436) & 1023] + 0.01177503f * fVec82[(IOTA0 - 435) & 1023] + 0.01182758f * fVec82[(IOTA0 - 434) & 1023] + 0.01213037f * fVec82[(IOTA0 - 433) & 1023] + 0.01234483f * fVec82[(IOTA0 - 432) & 1023] + 0.011161f * fVec82[(IOTA0 - 431) & 1023] + 0.0083245f * fVec82[(IOTA0 - 430) & 1023] + 0.00527355f * fVec82[(IOTA0 - 429) & 1023] + 0.00243069f * fVec82[(IOTA0 - 428) & 1023] + 0.00010936f * fVec82[(IOTA0 - 417) & 1023] + 0.00103034f * fVec82[(IOTA0 - 416) & 1023] + 0.00033225f * fVec82[(IOTA0 - 415) & 1023] + 0.00291006f * fVec82[(IOTA0 - 409) & 1023] + 0.00753598f * fVec82[(IOTA0 - 408) & 1023] + 0.0130808f * fVec82[(IOTA0 - 407) & 1023] + 0.01805363f * fVec82[(IOTA0 - 406) & 1023] + 0.02145577f * fVec82[(IOTA0 - 405) & 1023] + 0.02319057f * fVec82[(IOTA0 - 404) & 1023] + 0.02280786f * fVec82[(IOTA0 - 403) & 1023] + 0.0209995f * fVec82[(IOTA0 - 402) & 1023] + 0.01863186f * fVec82[(IOTA0 - 401) & 1023] + 0.01493963f * fVec82[(IOTA0 - 400) & 1023] + 0.00977544f * fVec82[(IOTA0 - 399) & 1023] + 0.00549228f * fVec82[(IOTA0 - 398) & 1023] + 0.00235713f * fVec82[(IOTA0 - 397) & 1023] + 3.36e-05f * fVec82[(IOTA0 - 396) & 1023] + 0.00478575f * fVec82[(IOTA0 - 386) & 1023] + 0.00871773f * fVec82[(IOTA0 - 385) & 1023] + 0.01119888f * fVec82[(IOTA0 - 384) & 1023] + 0.01468522f * fVec82[(IOTA0 - 383) & 1023] + 0.01662591f * fVec82[(IOTA0 - 382) & 1023] + 0.01724837f * fVec82[(IOTA0 - 381) & 1023] + 0.0185919f * fVec82[(IOTA0 - 380) & 1023] + 0.0191744f * fVec82[(IOTA0 - 379) & 1023] + 0.01821982f * fVec82[(IOTA0 - 378) & 1023] + 0.01745439f * fVec82[(IOTA0 - 377) & 1023] + 0.01848266f * fVec82[(IOTA0 - 376) & 1023] + 0.01935317f * fVec82[(IOTA0 - 375) & 1023] + 0.01899147f * fVec82[(IOTA0 - 374) & 1023] + 0.01796113f * fVec82[(IOTA0 - 373) & 1023] + 0.01565874f * fVec82[(IOTA0 - 372) & 1023] + 0.0072564f * fVec82[(IOTA0 - 370) & 1023] + 0.00215319f * fVec82[(IOTA0 - 354) & 1023] + 0.00782412f * fVec82[(IOTA0 - 353) & 1023] + 0.01214088f * fVec82[(IOTA0 - 352) & 1023] + 0.01418679f * fVec82[(IOTA0 - 351) & 1023] + 0.01346987f * fVec82[(IOTA0 - 350) & 1023] + 0.00895747f * fVec82[(IOTA0 - 349) & 1023] + 0.00236129f * fVec82[(IOTA0 - 348) & 1023] + 0.00021237f * fVec82[(IOTA0 - 335) & 1023] + 0.00230874f * fVec82[(IOTA0 - 334) & 1023] + 0.00686526f * fVec82[(IOTA0 - 333) & 1023] + 0.01069007f * fVec82[(IOTA0 - 332) & 1023] + 0.01533285f * fVec82[(IOTA0 - 331) & 1023] + 0.02184899f * fVec82[(IOTA0 - 330) & 1023] + 0.02601448f * fVec82[(IOTA0 - 329) & 1023] + 0.0281403f * fVec82[(IOTA0 - 328) & 1023] + 0.02953014f * fVec82[(IOTA0 - 327) & 1023] + 0.02867649f * fVec82[(IOTA0 - 326) & 1023] + 0.02510609f * fVec82[(IOTA0 - 325) & 1023] + 0.02013949f * fVec82[(IOTA0 - 324) & 1023] + 0.01526552f * fVec82[(IOTA0 - 323) & 1023] + 0.01028218f * fVec82[(IOTA0 - 322) & 1023] + 0.00704195f * fVec82[(IOTA0 - 321) & 1023] + 0.00651835f * fVec82[(IOTA0 - 320) & 1023] + 0.00581389f * fVec82[(IOTA0 - 319) & 1023] + 0.00501486f * fVec82[(IOTA0 - 318) & 1023] + 0.00380172f * fVec82[(IOTA0 - 317) & 1023] + 0.00142357f * fVec82[(IOTA0 - 305) & 1023] + 0.00061818f * fVec82[(IOTA0 - 304) & 1023] + 0.00052568f * fVec82[(IOTA0 - 279) & 1023] + 0.00458804f * fVec82[(IOTA0 - 278) & 1023] + 0.00652885f * fVec82[(IOTA0 - 277) & 1023] + 0.00673072f * fVec82[(IOTA0 - 276) & 1023] + 0.00662136f * fVec82[(IOTA0 - 275) & 1023] + 0.00399295f * fVec82[(IOTA0 - 274) & 1023] + 2.08e-06f * fVec82[(IOTA0 - 273) & 1023] + 0.00010509f * fVec82[(IOTA0 - 262) & 1023] + 0.00384155f * fVec82[(IOTA0 - 261) & 1023] + 0.00742674f * fVec82[(IOTA0 - 260) & 1023] + 0.00809954f * fVec82[(IOTA0 - 259) & 1023] + 0.00492872f * fVec82[(IOTA0 - 258) & 1023] + 0.00962612f * fVec82[(IOTA0 - 244) & 1023] + 0.01709489f * fVec82[(IOTA0 - 243) & 1023] + 0.02141801f * fVec82[(IOTA0 - 242) & 1023] + 0.02393278f * fVec82[(IOTA0 - 241) & 1023] + 0.02492316f * fVec82[(IOTA0 - 240) & 1023] + 0.02552876f * fVec82[(IOTA0 - 239) & 1023] + 0.02479278f * fVec82[(IOTA0 - 238) & 1023] + 0.02004491f * fVec82[(IOTA0 - 237) & 1023] + 0.01434454f * fVec82[(IOTA0 - 236) & 1023] + 0.00911095f * fVec82[(IOTA0 - 235) & 1023] + 0.00111234f * fVec82[(IOTA0 - 234) & 1023] + 0.00349464f * fVec82[(IOTA0 - 216) & 1023] + 0.01641574f * fVec82[(IOTA0 - 215) & 1023] + 0.03162651f * fVec82[(IOTA0 - 214) & 1023] + 0.04271823f * fVec82[(IOTA0 - 213) & 1023] + 0.04694665f * fVec82[(IOTA0 - 212) & 1023] + 0.04687517f * fVec82[(IOTA0 - 211) & 1023] + 0.03921088f * fVec82[(IOTA0 - 210) & 1023] + 0.02400218f * fVec82[(IOTA0 - 209) & 1023] + 0.00944112f * fVec82[(IOTA0 - 208) & 1023] + 0.00013038f * fVec82[(IOTA0 - 193) & 1023] + 0.02525321f * fVec82[(IOTA0 - 192) & 1023] + 0.04281489f * fVec82[(IOTA0 - 191) & 1023] + 0.05094388f * fVec82[(IOTA0 - 190) & 1023] + 0.05186914f * fVec82[(IOTA0 - 189) & 1023] + 0.04308824f * fVec82[(IOTA0 - 188) & 1023] + 0.02900654f * fVec82[(IOTA0 - 187) & 1023] + 0.01450217f * fVec82[(IOTA0 - 186) & 1023] + 0.00685902f * fVec82[(IOTA0 - 172) & 1023] + 0.03020295f * fVec82[(IOTA0 - 171) & 1023] + 0.04677423f * fVec82[(IOTA0 - 170) & 1023] + 0.05247046f * fVec82[(IOTA0 - 169) & 1023] + 0.05080507f * fVec82[(IOTA0 - 168) & 1023] + 0.03876304f * fVec82[(IOTA0 - 167) & 1023] + 0.01881478f * fVec82[(IOTA0 - 166) & 1023] + 0.00391523f * fVec82[(IOTA0 - 140) & 1023] + 0.00459855f * fVec82[(IOTA0 - 139) & 1023] + 0.00688835f * fVec82[(IOTA0 - 138) & 1023] + 0.0132911f * fVec82[(IOTA0 - 137) & 1023] + 0.01370742f * fVec82[(IOTA0 - 136) & 1023] + 0.01018333f * fVec82[(IOTA0 - 135) & 1023] + 0.00806166f * fVec82[(IOTA0 - 134) & 1023] + 0.00086636f * fVec82[(IOTA0 - 133) & 1023] + 0.00083691f * fVec82[(IOTA0 - 112) & 1023] + 0.00648047f * fVec82[(IOTA0 - 111) & 1023] + 0.0088145f * fVec82[(IOTA0 - 110) & 1023] + 0.01300724f * fVec82[(IOTA0 - 109) & 1023] + 0.01715795f * fVec82[(IOTA0 - 108) & 1023] + 0.01969373f * fVec82[(IOTA0 - 107) & 1023] + 0.01580794f * fVec82[(IOTA0 - 106) & 1023] + 0.00112077f * fVec82[(IOTA0 - 96) & 1023] + 0.01561451f * fVec82[(IOTA0 - 95) & 1023] + 0.03373339f * fVec82[(IOTA0 - 94) & 1023] + 0.03964822f * fVec82[(IOTA0 - 93) & 1023] + 0.04061123f * fVec82[(IOTA0 - 92) & 1023] + 0.04368748f * fVec82[(IOTA0 - 91) & 1023] + 0.03895012f * fVec82[(IOTA0 - 90) & 1023] + 0.0235943f * fVec82[(IOTA0 - 89) & 1023] + 0.00939896f * fVec82[(IOTA0 - 88) & 1023] + 0.01450852f * fVec82[(IOTA0 - 77) & 1023] + 0.04190869f * fVec82[(IOTA0 - 76) & 1023] + 0.04424261f * fVec82[(IOTA0 - 75) & 1023] + 0.04571664f * fVec82[(IOTA0 - 74) & 1023] + 0.05205829f * fVec82[(IOTA0 - 73) & 1023] + 0.04392087f * fVec82[(IOTA0 - 72) & 1023] + 0.05189639f * fVec82[(IOTA0 - 71) & 1023] + 0.09826909f * fVec82[(IOTA0 - 70) & 1023] + 0.13435327f * fVec82[(IOTA0 - 69) & 1023] + 0.16406633f * fVec82[(IOTA0 - 68) & 1023] + 0.20976403f * fVec82[(IOTA0 - 67) & 1023] + 0.22017659f * fVec82[(IOTA0 - 66) & 1023] + 0.18572396f * fVec82[(IOTA0 - 65) & 1023] + 0.14658248f * fVec82[(IOTA0 - 64) & 1023] + 0.10157873f * fVec82[(IOTA0 - 63) & 1023] + 0.04815149f * fVec82[(IOTA0 - 62) & 1023] + 0.0087556f * fVec82[(IOTA0 - 61) & 1023] + 0.02732441f * fVec82[(IOTA0 - 58) & 1023] + 0.08225916f * fVec82[(IOTA0 - 57) & 1023] + 0.10226632f * fVec82[(IOTA0 - 56) & 1023] + 0.12682143f * fVec82[(IOTA0 - 55) & 1023] + 0.15040095f * fVec82[(IOTA0 - 54) & 1023] + 0.13561492f * fVec82[(IOTA0 - 53) & 1023] + 0.11354084f * fVec82[(IOTA0 - 52) & 1023] + 0.0973018f * fVec82[(IOTA0 - 51) & 1023] + 0.08243793f * fVec82[(IOTA0 - 50) & 1023] + 0.09546619f * fVec82[(IOTA0 - 49) & 1023] + 0.13599762f * fVec82[(IOTA0 - 48) & 1023] + 0.18808745f * fVec82[(IOTA0 - 47) & 1023] + 0.22771464f * fVec82[(IOTA0 - 46) & 1023] + 0.2334529f * fVec82[(IOTA0 - 45) & 1023] + 0.22357653f * fVec82[(IOTA0 - 44) & 1023] + 0.19581063f * fVec82[(IOTA0 - 43) & 1023] + 0.1365359f * fVec82[(IOTA0 - 42) & 1023] + 0.07028227f * fVec82[(IOTA0 - 41) & 1023] + 0.01978f * fVec82[(IOTA0 - 40) & 1023] + 0.02666212f * fVec82[(IOTA0 - 35) & 1023] + 0.09738379f * fVec82[(IOTA0 - 34) & 1023] + 0.16323993f * fVec82[(IOTA0 - 33) & 1023] + 0.19997808f * fVec82[(IOTA0 - 32) & 1023] + 0.2425008f * fVec82[(IOTA0 - 31) & 1023] + 0.3138239f * fVec82[(IOTA0 - 30) & 1023] + 0.33859348f * fVec82[(IOTA0 - 29) & 1023] + 0.37667736f * fVec82[(IOTA0 - 28) & 1023] + 0.48415834f * fVec82[(IOTA0 - 27) & 1023] + 0.53571194f * fVec82[(IOTA0 - 26) & 1023] + 0.628348f * fVec82[(IOTA0 - 25) & 1023] + 0.7170625f * fVec82[(IOTA0 - 24) & 1023] + 0.56004846f * fVec82[(IOTA0 - 23) & 1023] + 0.22003986f * fVec82[(IOTA0 - 21) & 1023] + 0.35309994f * fVec82[(IOTA0 - 22) & 1023] + 0.29353094f * fVec81[(IOTA0 - 8) & 1023]))));
			output0[i0] = static_cast<FAUSTFLOAT>(fTemp220);
			output1[i0] = static_cast<FAUSTFLOAT>(fTemp220);
			fRec0[1] = fRec0[0];
			fRec1[1] = fRec1[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec11[1] = fRec11[0];
			fRec13[1] = fRec13[0];
			fRec12[1] = fRec12[0];
			fRec14[1] = fRec14[0];
			fRec18[1] = fRec18[0];
			fRec20[1] = fRec20[0];
			fVec0[1] = fVec0[0];
			fRec23[1] = fRec23[0];
			fRec22[1] = fRec22[0];
			fRec21[2] = fRec21[1];
			fRec21[1] = fRec21[0];
			fRec19[1] = fRec19[0];
			fRec17[2] = fRec17[1];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			fRec26[1] = fRec26[0];
			fRec25[2] = fRec25[1];
			fRec25[1] = fRec25[0];
			fRec24[1] = fRec24[0];
			fRec29[1] = fRec29[0];
			fRec28[2] = fRec28[1];
			fRec28[1] = fRec28[0];
			fRec27[1] = fRec27[0];
			fRec32[1] = fRec32[0];
			fRec31[2] = fRec31[1];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec35[1] = fRec35[0];
			fRec34[2] = fRec34[1];
			fRec34[1] = fRec34[0];
			fRec33[1] = fRec33[0];
			fRec38[1] = fRec38[0];
			fRec37[2] = fRec37[1];
			fRec37[1] = fRec37[0];
			fRec36[1] = fRec36[0];
			fRec15[1] = fRec15[0];
			fRec42[1] = fRec42[0];
			fVec1[1] = fVec1[0];
			fRec43[1] = fRec43[0];
			fRec41[2] = fRec41[1];
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fRec46[1] = fRec46[0];
			fRec45[2] = fRec45[1];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec49[1] = fRec49[0];
			fRec48[2] = fRec48[1];
			fRec48[1] = fRec48[0];
			fRec47[1] = fRec47[0];
			fRec52[1] = fRec52[0];
			fRec51[2] = fRec51[1];
			fRec51[1] = fRec51[0];
			fRec50[1] = fRec50[0];
			fRec55[1] = fRec55[0];
			fRec54[2] = fRec54[1];
			fRec54[1] = fRec54[0];
			fRec53[1] = fRec53[0];
			fRec58[1] = fRec58[0];
			fRec57[2] = fRec57[1];
			fRec57[1] = fRec57[0];
			fRec56[1] = fRec56[0];
			fRec39[1] = fRec39[0];
			fRec61[1] = fRec61[0];
			fVec2[1] = fVec2[0];
			fRec62[1] = fRec62[0];
			fRec60[2] = fRec60[1];
			fRec60[1] = fRec60[0];
			fRec59[1] = fRec59[0];
			fRec65[1] = fRec65[0];
			fRec64[2] = fRec64[1];
			fRec64[1] = fRec64[0];
			fRec63[1] = fRec63[0];
			fRec68[1] = fRec68[0];
			fRec67[2] = fRec67[1];
			fRec67[1] = fRec67[0];
			fRec66[1] = fRec66[0];
			fRec71[1] = fRec71[0];
			fRec70[2] = fRec70[1];
			fRec70[1] = fRec70[0];
			fRec69[1] = fRec69[0];
			fRec74[1] = fRec74[0];
			fRec73[2] = fRec73[1];
			fRec73[1] = fRec73[0];
			fRec72[1] = fRec72[0];
			fRec77[1] = fRec77[0];
			fRec76[2] = fRec76[1];
			fRec76[1] = fRec76[0];
			fRec75[1] = fRec75[0];
			fVec3[1] = fVec3[0];
			fRec10[1] = fRec10[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			fRec79[1] = fRec79[0];
			fRec78[2] = fRec78[1];
			fRec78[1] = fRec78[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fVec4[1] = fVec4[0];
			fRec7[1] = fRec7[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec81[1] = fRec81[0];
			fRec80[2] = fRec80[1];
			fRec80[1] = fRec80[0];
			for (int j0 = 3; j0 > 0; j0 = j0 - 1) {
				fRec5[j0] = fRec5[j0 - 1];
			}
			for (int j1 = 3; j1 > 0; j1 = j1 - 1) {
				fRec82[j1] = fRec82[j1 - 1];
			}
			for (int j2 = 3; j2 > 0; j2 = j2 - 1) {
				fRec83[j2] = fRec83[j2 - 1];
			}
			for (int j3 = 3; j3 > 0; j3 = j3 - 1) {
				fRec84[j3] = fRec84[j3 - 1];
			}
			fRec88[1] = fRec88[0];
			fRec89[1] = fRec89[0];
			fRec91[2] = fRec91[1];
			fRec91[1] = fRec91[0];
			fRec90[2] = fRec90[1];
			fRec90[1] = fRec90[0];
			fRec92[1] = fRec92[0];
			fRec93[1] = fRec93[0];
			fRec94[1] = fRec94[0];
			fRec95[1] = fRec95[0];
			fVec5[1] = fVec5[0];
			fRec87[1] = fRec87[0];
			fRec86[1] = fRec86[0];
			fRec85[1] = fRec85[0];
			fRec96[1] = fRec96[0];
			fVec6[1] = fVec6[0];
			fRec2[1] = fRec2[0];
			fRec97[1] = fRec97[0];
			fRec98[1] = fRec98[0];
			fRec110[1] = fRec110[0];
			fRec109[1] = fRec109[0];
			IOTA0 = IOTA0 + 1;
			fRec107[1] = fRec107[0];
			fRec114[1] = fRec114[0];
			fRec113[1] = fRec113[0];
			fRec111[1] = fRec111[0];
			fRec118[1] = fRec118[0];
			fRec117[1] = fRec117[0];
			fRec115[1] = fRec115[0];
			fRec122[1] = fRec122[0];
			fRec121[1] = fRec121[0];
			fRec119[1] = fRec119[0];
			fRec126[1] = fRec126[0];
			fRec125[1] = fRec125[0];
			fRec123[1] = fRec123[0];
			fRec130[1] = fRec130[0];
			fRec129[1] = fRec129[0];
			fRec127[1] = fRec127[0];
			fRec134[1] = fRec134[0];
			fRec133[1] = fRec133[0];
			fRec131[1] = fRec131[0];
			fRec138[1] = fRec138[0];
			fRec137[1] = fRec137[0];
			fRec135[1] = fRec135[0];
			fRec99[2] = fRec99[1];
			fRec99[1] = fRec99[0];
			fRec100[2] = fRec100[1];
			fRec100[1] = fRec100[0];
			fRec101[2] = fRec101[1];
			fRec101[1] = fRec101[0];
			fRec102[2] = fRec102[1];
			fRec102[1] = fRec102[0];
			fRec103[2] = fRec103[1];
			fRec103[1] = fRec103[0];
			fRec104[2] = fRec104[1];
			fRec104[1] = fRec104[0];
			fRec105[2] = fRec105[1];
			fRec105[1] = fRec105[0];
			fRec106[2] = fRec106[1];
			fRec106[1] = fRec106[0];
			fRec148[1] = fRec148[0];
			fRec140[1] = fRec140[0];
			fRec149[1] = fRec149[0];
			fRec141[1] = fRec141[0];
			fRec150[1] = fRec150[0];
			fRec142[1] = fRec142[0];
			fRec151[1] = fRec151[0];
			fRec143[1] = fRec143[0];
			fRec152[1] = fRec152[0];
			fRec144[1] = fRec144[0];
			fRec153[1] = fRec153[0];
			fRec145[1] = fRec145[0];
			fRec154[1] = fRec154[0];
			fRec146[1] = fRec146[0];
			fRec155[1] = fRec155[0];
			fRec147[1] = fRec147[0];
			fVec56[1] = fVec56[0];
			fRec139[1] = fRec139[0];
			fRec166[1] = fRec166[0];
			fRec164[1] = fRec164[0];
			fRec168[1] = fRec168[0];
			fRec167[1] = fRec167[0];
			fRec170[1] = fRec170[0];
			fRec169[1] = fRec169[0];
			fRec172[1] = fRec172[0];
			fRec171[1] = fRec171[0];
			fRec174[1] = fRec174[0];
			fRec173[1] = fRec173[0];
			fRec176[1] = fRec176[0];
			fRec175[1] = fRec175[0];
			fRec178[1] = fRec178[0];
			fRec177[1] = fRec177[0];
			fRec180[1] = fRec180[0];
			fRec179[1] = fRec179[0];
			fRec162[1] = fRec162[0];
			fRec160[1] = fRec160[0];
			fRec158[1] = fRec158[0];
			fRec156[1] = fRec156[0];
			fRec190[1] = fRec190[0];
			fRec189[1] = fRec189[0];
			fRec192[1] = fRec192[0];
			fRec191[1] = fRec191[0];
			fRec194[1] = fRec194[0];
			fRec193[1] = fRec193[0];
			fRec196[1] = fRec196[0];
			fRec195[1] = fRec195[0];
			fRec198[1] = fRec198[0];
			fRec197[1] = fRec197[0];
			fRec200[1] = fRec200[0];
			fRec199[1] = fRec199[0];
			fRec202[1] = fRec202[0];
			fRec201[1] = fRec201[0];
			fRec204[1] = fRec204[0];
			fRec203[1] = fRec203[0];
			fRec187[1] = fRec187[0];
			fRec185[1] = fRec185[0];
			fRec183[1] = fRec183[0];
			fRec181[1] = fRec181[0];
		}
	}

};

/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

typedef sf_count_t (* sample_read)(SNDFILE* sndfile, void* buffer, sf_count_t frames);
typedef sf_count_t (* sample_write)(SNDFILE* sndfile, void* buffer, sf_count_t frames);

using namespace std;

// loptrm : scan command-line arguments and remove and return long int value when found
static long loptrm(int* argcP, char* argv[], const char* longname, const char* shortname, long def)
{
    int argc = *argcP;
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i-1], shortname) == 0 || strcmp(argv[i-1], longname) == 0) {
            int optval = atoi(argv[i]);
            for (int j = i-1; j < argc-2; j++) {  // make it go away for sake of "faust/gui/console.h"
                argv[j] = argv[j+2];
            }
            *argcP -= 2;
            return optval;
        }
    }
    return def;
}

mydsp DSP;

std::list<GUI*> GUI::fGuiList;
ztimedmap GUI::gTimedZoneMap;

#define kBufferSize  64
#define kSampleRate  44100

int main(int argc_aux, char* argv_aux[])
{
    char name[256];
    char rcfilename[256];
    char* home = getenv("HOME");
    snprintf(name, 256, "%s", basename(argv_aux[0]));
    snprintf(rcfilename, 256, "%s/.%src", home, name);
    string cfilename;
    
    int argc = 0;
    char* argv[64];
    for (int i = 0; i < argc_aux; i++) {
        if (string(argv_aux[i]) == "-ct") {
            cfilename = argv_aux[i+1];
            i++;
            continue;
        }
        argv[argc++] = argv_aux[i];
    }
    
    // Recall state before handling commands
    FUI finterface;
    DSP.buildUserInterface(&finterface);
    
    CMDUI* interface = new CMDUI(argc, argv, true);
    DSP.buildUserInterface(interface);
    
    interface->process_command(FILE_MODE);
    interface->printhelp_command(FILE_MODE);
    
    sample_read reader;
    sample_write writer;
    if (sizeof(FAUSTFLOAT) == 4) {
        reader = reinterpret_cast<sample_read>(sf_readf_float);
        writer = reinterpret_cast<sample_write>(sf_writef_float);
    } else {
        reader = reinterpret_cast<sample_read>(sf_readf_double);
        writer = reinterpret_cast<sample_write>(sf_writef_double);
    }
    
    bool is_rc = loptrm(&argc, argv, "--rcfile", "-rc", 0);
    
    int buffer_size = loptrm(&argc, argv, "--buffer-size", "-bs", kBufferSize);
    
    if (FILE_MODE == INPUT_OUTPUT_FILE) {
        
        int num_samples = loptrm(&argc, argv, "--continue", "-c", 0);
        
        SF_INFO in_info;
        memset(&in_info, 0, sizeof(in_info));
        SNDFILE* in_sf = sf_open(interface->input_file(), SFM_READ, &in_info);
        if (!in_sf) {
            cerr << "ERROR : input file not found" << endl;
            sf_perror(in_sf);
            exit(1);
        }
        
        SF_INFO out_info = in_info;
        out_info.format = in_info.format;
        out_info.channels = DSP.getNumOutputs();
        SNDFILE* out_sf = sf_open(interface->output_file(), SFM_WRITE, &out_info);
        if (!out_sf) {
            cerr << "ERROR : cannot write output file" << endl;
            sf_perror(out_sf);
            exit(1);
        }
        
        // Handling of the file containing sequence of time-stamped OSC messages
        ControlSequenceUI sequenceUI(OSCSequenceReader::read(cfilename, in_info.samplerate));
        DSP.buildUserInterface(&sequenceUI);
        
        // Init DSP with SR
        DSP.init(in_info.samplerate);
        
        // Possibly restore saved state
        if (is_rc) {
            finterface.recallState(rcfilename);
        }
        
        // Modify the UI values according to the command line options, after init
        interface->process_init();
        
        // Create deinterleaver and interleaver
        Deinterleaver dilv(buffer_size, in_info.channels, DSP.getNumInputs());
        Interleaver ilv(buffer_size, DSP.getNumOutputs(), DSP.getNumOutputs());
        
        // Process all samples
        int nbf;
        uint64_t cur_frame = 0;
        do {
            // Read samples
            nbf = reader(in_sf, dilv.input(), buffer_size);
            dilv.deinterleave();
            // Update controllers
            sequenceUI.process(cur_frame, cur_frame + nbf);
            cur_frame += nbf;
            // Compute DSP
            DSP.compute(nbf, dilv.outputs(), ilv.inputs());
            // Write samples
            ilv.interleave();
            writer(out_sf, ilv.output(), nbf);
        } while (nbf == buffer_size);
        
        sf_close(in_sf);
        
        // Compute tail, if any
        if (num_samples > 0) {
            FAUSTFLOAT** inputs = (FAUSTFLOAT**)alloca(DSP.getNumInputs() * sizeof(FAUSTFLOAT*));
            for (int chan = 0; chan < DSP.getNumInputs(); chan++) {
                inputs[chan] = (FAUSTFLOAT*)alloca(num_samples * sizeof(FAUSTFLOAT));
                memset(inputs[chan], 0, num_samples * sizeof(FAUSTFLOAT));
            }
            Interleaver ilv(num_samples, DSP.getNumOutputs(), DSP.getNumOutputs());
            DSP.compute(num_samples, inputs, ilv.inputs());
            ilv.interleave();
            writer(out_sf, ilv.output(), num_samples);
        }
        
        sf_close(out_sf);
        
        // Possibly save state
        if (is_rc) {
            finterface.saveState(rcfilename);
        }
        
    } else {
        
        int sample_rate = loptrm(&argc, argv, "--sample-rate", "-sr", kSampleRate);
        
        // Handling of the file containing sequence of time-stamped OSC messages
        ControlSequenceUI sequenceUI(OSCSequenceReader::read(cfilename, sample_rate));
        uint64_t begin, end;
        sequenceUI.getRange(begin, end);
        DSP.buildUserInterface(&sequenceUI);
        
        int num_samples = loptrm(&argc, argv, "--samples", "-s", ((end > 0) ? (end + kSampleRate) : kSampleRate * 5));
        int bit_depth = loptrm(&argc, argv, "--bith-depth (16|24|32)", "-bd", 16);
        int bd = (bit_depth == 16) ? SF_FORMAT_PCM_16 : ((bit_depth == 24) ? SF_FORMAT_PCM_24 : SF_FORMAT_PCM_32);
        
        SF_INFO out_info = { num_samples, sample_rate, DSP.getNumOutputs(), SF_FORMAT_WAV|bd|SF_ENDIAN_LITTLE, 0, 0};
        SNDFILE* out_sf = sf_open(interface->input_file(), SFM_WRITE, &out_info);
        if (!out_sf) {
            cerr << "ERROR : cannot write output file" << endl;
            sf_perror(out_sf);
            exit(1);
        }
        
        // Init DSP with SR
        DSP.init(sample_rate);
        
        // Possibly restore saved state
        if (is_rc) {
            finterface.recallState(rcfilename);
        }
        
        // Modify the UI values according to the command line options, after init
        interface->process_init();
        
        // Create interleaver
        Interleaver ilv(buffer_size, DSP.getNumOutputs(), DSP.getNumOutputs());
        
        // Process all samples
        uint64_t cur_frame = 0;
        do {
            int nbf = std::min(int(num_samples - cur_frame), int(buffer_size));
            // Update controllers
            sequenceUI.process(cur_frame, cur_frame + nbf);
            // Compute DSP
            DSP.compute(nbf, nullptr, ilv.inputs());
            // Write samples
            ilv.interleave();
            writer(out_sf, ilv.output(), nbf);
            cur_frame += nbf;
        } while (cur_frame < num_samples);
        
        sf_close(out_sf);
        
        // Possibly save state
        if (is_rc) {
            finterface.saveState(rcfilename);
        }
    }
}

/******************* END sndfile.cpp ****************/

#endif
