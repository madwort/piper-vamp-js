/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    VamPipe

    Centre for Digital Music, Queen Mary, University of London.
    Copyright 2015-2016 QMUL.
  
    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the names of the Centre for
    Digital Music; Queen Mary, University of London; and Chris Cannam
    shall not be used in advertising or otherwise to promote the sale,
    use or other dealings in this Software without prior written
    authorization.
*/

#ifndef VAMPIPE_ADAPTER_H
#define VAMPIPE_ADAPTER_H

#include <vamp-hostsdk/PluginStaticData.h>
#include <vamp-hostsdk/PluginConfiguration.h>
#include <vamp-hostsdk/RequestResponse.h>

#include <vamp-hostsdk/PluginInputDomainAdapter.h>
#include <vamp-hostsdk/PluginBufferingAdapter.h>
#include <vamp-hostsdk/PluginChannelAdapter.h>

namespace vampipe {

class VamPipeAdapterBase
{
public:
    virtual Vamp::HostExt::PluginStaticData getStaticData() const = 0;
    virtual Vamp::HostExt::LoadResponse loadPlugin(Vamp::HostExt::LoadRequest r) const = 0;
};

template <typename P>
class VamPipeAdapter : public VamPipeAdapterBase
{
    const int adaptInputDomain = 0x01;
    const int adaptChannelCount = 0x02;
    const int adaptBufferSize = 0x04;
    
public:
    VamPipeAdapter(std::string libname) :
	m_soname(libname) { }
    
    virtual Vamp::HostExt::PluginStaticData getStaticData() const {
	P p(44100.f);
	return Vamp::HostExt::PluginStaticData::fromPlugin
	    (m_soname + ":" + p.getIdentifier(),
	     {}, //!!! todo: category - tricky one that
	     &p);
    }

    virtual Vamp::HostExt::LoadResponse loadPlugin(Vamp::HostExt::
						   LoadRequest r) const {
	
	// We assume the caller has guaranteed that the request is for
	// the correct plugin (so we don't have to check the plugin
	// key field here)

	Vamp::Plugin *p = new P(r.inputSampleRate);
	
	if (r.adapterFlags & adaptInputDomain) {
	    if (p->getInputDomain() == Vamp::Plugin::FrequencyDomain) {
		p = new Vamp::HostExt::PluginInputDomainAdapter(p);
	    }
	}

	if (r.adapterFlags & adaptBufferSize) {
	    p = new Vamp::HostExt::PluginBufferingAdapter(p);
	}

	if (r.adapterFlags & adaptChannelCount) {
	    p = new Vamp::HostExt::PluginChannelAdapter(p);
	}

	Vamp::HostExt::LoadResponse response;
	response.plugin = p;

	response.staticData = Vamp::HostExt::PluginStaticData::fromPlugin
	    (m_soname + ":" + p->getIdentifier(),
	     {}, //!!! todo: category - tricky one that
	     p);

	int defaultChannels = 0;
	if (p->getMinChannelCount() == p->getMaxChannelCount()) {
	    defaultChannels = p->getMinChannelCount();
	}
    
	response.defaultConfiguration = Vamp::HostExt::PluginConfiguration::fromPlugin
	    (p,
	     defaultChannels,
	     p->getPreferredStepSize(),
	     p->getPreferredBlockSize());
    
	return response;
    }
    
private:
    std::string m_soname;
};

}

#endif

