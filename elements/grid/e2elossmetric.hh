#ifndef E2EMETRIC_HH
#define E2EMETRIC_HH
#include <click/element.hh>
#include "gridgenericmetric.hh"
CLICK_DECLS

/*
 * =c
 * E2ELossMetric(LinkStat, [, I<KEYWORDS>])
 * =s Grid
 * =io
 * None
 * =d
 *
 * Child class of GridGenericMetric that implements the end-to-end
 * cumulative link loss ratio metric.  The metric is the product of
 * the delivery ratios of each link in the path, from 0--100 as a
 * percentage.  Larger metrics values are better.
 *
 * LinkStat is this node's LinkStat element, which is needed to obtain
 * the link delivery ratios used to calculate the metric.
 *
 * Keywords arguments are:
 *
 * =over 8
 *
 * =item TWOWAY
 *
 * Boolean.  If true, multiply the delivery ratios in both directions
 * for each link.  If false, only use the forward delivery ratio.
 * Defaults to false.
 *
 * =back
 *
 * =a HopcountMetric, LinkStat, ETXMetric */

class LinkStat;

class E2ELossMetric : public GridGenericMetric {
  
public:

  E2ELossMetric();
  ~E2ELossMetric();

  const char *class_name() const { return "E2ELossMetric"; }
  const char *processing() const { return AGNOSTIC; }
  E2ELossMetric *clone()  const { return new E2ELossMetric; } 

  int configure(Vector<String> &, ErrorHandler *);
  bool can_live_reconfigure() const { return false; }

  void add_handlers();

  void *cast(const char *);

  // generic metric methods
  bool metric_val_lt(const metric_t &, const metric_t &) const;
  metric_t get_link_metric(const EtherAddress &, bool) const;
  metric_t append_metric(const metric_t &, const metric_t &) const;
  metric_t prepend_metric(const metric_t &r, const metric_t &l) const 
  { return append_metric(r, l); }

private:
  LinkStat *_ls;
  bool _twoway;
};

CLICK_ENDDECLS
#endif