import test from 'tape'

import _multifunc from '../../../../src/js-context/functions/types/_multifunc'

test('_multifunc', t => {
  let m1 = _multifunc('m1', {
    null: () => 'dispatched to null',
    integer: () => 'dispatched to integer',
    foo: () => 'dispatched to foo'
  }, () => 'dispatched to default')

  t.equal(m1(null), 'dispatched to null')
  t.equal(m1({type: 'foo'}), 'dispatched to foo')
  t.equal(m1(3.14), 'dispatched to default')

  let m2 = _multifunc('m2', {
    'null, null': () => 'dispatched to null, null',
    'integer, float': () => 'dispatched to integer, float',
    'foo, integer': () => 'dispatched to foo, integer'
  }, 2)

  t.equal(m2(null, null), 'dispatched to null, null')
  t.equal(m2(1, 3.14), 'dispatched to integer, float')
  t.equal(m2({type: 'foo'}, 42), 'dispatched to foo, integer')  
  t.throws(() => m2(), /Unable to dispatch function call "m2\(\)"/)
  t.throws(() => m2(3.14), /Unable to dispatch function call "m2\(float\)"/)

  t.end()
})
